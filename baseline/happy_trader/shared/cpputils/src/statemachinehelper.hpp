#ifndef _CPPUTILS_HSM_INCLUDED
#define _CPPUTILS_HSM_INCLUDED

#include "cpputilsdefs.hpp"

#include <functional>
using namespace std;

#include "uid.hpp"
#include "template.hpp"
#include "assertdef.hpp"
#include "queue"
#include "except.hpp"



namespace CppUtils {
 // ------------------------------------
	// STATE MACHINE EXCEPTION
	// this exception is necessary to notify that we are in a fatal state and all processing must be stopped
	// machine can't recover from this state and ignores all consequent processing
	class CPPUTILS_EXP StateMachineException: public CppUtils::Exception
	{
	public:
		StateMachineException (
			String const &msg,
			String const &ctx,
			String const &arg
			);
		virtual String addInfo() const;
		
		String type () const;

		/** Exception type description. */
		static const String exceptionType_scm;

	};

 // ------------------------------------

#define FSM_ASSERT(COND) \
	if (!(COND)) {	\
		THROW(CppUtils::OperationFailed, "exc_FSM_Error", "ctx_FSM", #COND );	\
	};

#define FSM_SOFT_ASSERT(COND) \
	if (!(COND)) {	\
		LOG(MSG_WARN, "FSM", #COND)	\
	};
	

#define CTOR_ID(CLASS, ID)	\
	static const int CLASS_ID = ID;	\
	virtual int getClassId()	const \
	{	\
		return CLASS_ID;	\
	};	\
	virtual char const* getClassName() const	\
	{	\
		static const char* name = #CLASS;	\
		return name;	\
	};	\
	virtual CppUtils::String const& getClassName2() const	\
	{	\
		static const CppUtils::String name = #CLASS;	\
		return name;	\
	};	\
	typedef CLASS DerivedClassName;


#define CTOR(CLASS) CTOR_ID(CLASS, __COUNTER__)




	// ----------
	// base class to allow to keep class ID functionality
	class BaseIdent {
		public:
			virtual int getClassId() const = 0;

			virtual char const* getClassName() const = 0;

			virtual CppUtils::String const& getClassName2() const = 0;

			virtual CppUtils::String toString() const	 = 0;

			//	dtor
			virtual ~BaseIdent()
			{
			}
	
	};
	// ---------------------------

	template<class CONTEXT> class StateMachine;
	template<class CONTEXT> class BaseState;

	class BaseEvent;

	// ---------------------------
	// Action on transition - must look like functor
	template<class CONTEXT>
	class BaseTransitionAction: public BaseIdent
	{
		friend class BaseState<CONTEXT>;
		public:
			inline StateMachine<CONTEXT>& machine()
			{
				return *machine_m;
			}

			inline StateMachine<CONTEXT> const& machine() const
			{
				return *machine_m;
			}

			virtual bool execute(BaseEvent const& eventToProcess)
			{
				return false;
			}

			virtual CppUtils::String toString() const
			{
				CppUtils::String result;
				result = CppUtils::String(" TRANSITION ACTION [ ") + getClassName2() + " ]"; 

				return result;
			}
		private:

			StateMachine<CONTEXT>* machine_m;

	};

	// ------------

	template<class CONTEXT> class StateMachine;
	
	

	// base actioons classes
	class ActionType {
	public:
		enum A_Type
		{
			ENTRY = 1,
			EXIT =	2
		};
	};

#define BEGIN_EVENT_MAP()	 \
	virtual bool execute(CppUtils::BaseEvent const& eventToProcess) {	


#define EVENT_MAP(EVENT_CLASS, FUN)	\
		if(eventToProcess.getClassId() == EVENT_CLASS::CLASS_ID)		\
			return DerivedClassName::FUN((EVENT_CLASS const&)eventToProcess);

#define END_EVENT_MAP()	\
	return executeDefaultHandler(eventToProcess);	\
	};

	template<class CONTEXT>
	class BaseAction: public BaseIdent {
		friend class BaseState<CONTEXT>;
		
		public:

			virtual ~BaseAction()
			{
			}


			// if not derived - no mapping is used
			virtual bool execute(BaseEvent const& eventToProcess)
			{
				// default handler
				return executeDefaultHandler(eventToProcess);
			}

			// can be derived
			virtual bool executeDefaultHandler(BaseEvent const& eventToProcess)
			{
				return false;
			};


			virtual CppUtils::String toString() const
			{
				CppUtils::String result;
				result = CppUtils::String(" ACTION [ ") + getClassName2() + " ]"; 

				return result;
			}

			CppUtils::ActionType::A_Type getType() const
			{		
				return type_m;
			};

		

		protected:

			BaseAction():
				state_m(NULL)
			{
				
			}

			inline BaseState<CONTEXT>& getState()
			{
				return *state_m;
			}

			inline CONTEXT& getContext()
			{
				return  getState().context();
			}

			ActionType::A_Type type_m;

		private:
			BaseState<CONTEXT>* state_m;

			
	};

	template<class CONTEXT>
	class BaseEnterAction: public BaseAction<CONTEXT>
	{
	public:
		BaseEnterAction()
		{
			type_m = ActionType::ENTRY;
		}

		virtual ~BaseEnterAction()
		{
			
		}

	};

	template<class CONTEXT>
	class BaseExitAction: public BaseAction<CONTEXT>
	{
	public:
		BaseExitAction()
		{
			type_m = ActionType::EXIT;
		}

		virtual ~BaseExitAction()
		{
		}

	protected:

	};
	

	// ----------
	class BaseEvent: public BaseIdent {
	public:

		virtual ~BaseEvent()
		{
		}

		BaseEvent()
		{
			uid_m.generate();
		}

		inline Uid const& getEventUid() const
		{
			return uid_m;
		}

		// this must be derived when receiving events previously registered for sending
		// returns true if want to remove this events from future delivery
		virtual bool mailed_event_arrived(BaseEvent const& event_arrived)
		{
			return true;
		}

		virtual CppUtils::String toString() const
		{
			CppUtils::String result;
			result = CppUtils::String(" [ ") + getClassName2() + " ]"; 

			return result;
		}


	private:
		Uid uid_m;
	};

	// -------------------------------
	class NullEvent: public BaseEvent
	{
	public:
		CTOR_ID(NullEvent, -1);

		NullEvent()
		{
		}
	};

	// -------------------------------

	class Unconditional {
	public:
		bool operator()(...) 
		{ 
			return true; 
		}
	};

	// ------------------------------

	

	template<class CONTEXT>
	class BaseState: public BaseIdent {
		
		
	public:

		typedef CONTEXT ContexType;

		enum TransitionType
		{
			TT_Guarded = 1,
			TT_Triggered  =2
		};

			BaseState():
				context_(NULL),
				machine_(NULL)
			{
			
			}

			virtual ~BaseState()
			{
				for(int i = 0; i < entry_actions_.size(); i++) {
					delete entry_actions_[i];
				}

				entry_actions_.clear();

				for(int i = 0; i < exit_actions_.size(); i++) {
					delete exit_actions_[i];
				}

				exit_actions_.clear();

				for(int i = 0; i < outgoing_.size(); i++) {
					delete outgoing_[i];
				}
			}

			// template versions
			template<class A>
			A* add_entry_action()
			{
				BaseAction<CONTEXT>* action = new A();
				add_entry_action(action);

				return static_cast<A*>(action);
			}

			template<class A>
			A* add_exit_action()
			{
				BaseAction<CONTEXT>* action = new A();
				add_exit_action(action);

				return static_cast<A*>(action);
			}

			// non-template versions
			void add_entry_action(BaseAction<CONTEXT>* action)
			{
			
				FSM_ASSERT(action->getType() == ActionType::ENTRY && machine_ != NULL);
				action->state_m = this;
				entry_actions_.push_back(action);
				machine_->mapped_actions_[action->getClassId()] = action;
			}

	
			void add_exit_action(BaseAction<CONTEXT>* action)
			{
			
				FSM_ASSERT(action->getType() == ActionType::EXIT && machine_ != NULL);
				action->state_m = this;
				exit_actions_.push_back(action);
				machine_->mapped_actions_[action->getClassId()] = action;
			}


			virtual CppUtils::String toString() const
			{
				CppUtils::String result;
				result = CppUtils::String(" [ ") + getClassName2() + " ] "; 

				return result;
			}


			
			template<class E_TYPE> void connect(BaseState<CONTEXT> &target, BaseTransitionAction<CONTEXT>* transition_action = 0) 
			{
				FSM_ASSERT(machine_ != NULL);
				if (transition_action)
					transition_action->machine_m = machine_;
				new TriggeredTransition(this, &target, E_TYPE::CLASS_ID,  transition_action); 
				
			}

			// добавить переход по условию
			template<class Predicate> void connect(BaseState<CONTEXT> &target, const Predicate &guard, BaseTransitionAction<CONTEXT>* transition_action = 0) 
			{
				FSM_ASSERT(machine_ != NULL);
				if (transition_action)
					transition_action->machine_m = machine_;
				new GuardedTransition<Predicate>(this, &target, guard, transition_action);
			}

			template<class E_TYPE, class TRANS_ACTION> void connect(BaseState<CONTEXT> &target) 
			{
				BaseTransitionAction<CONTEXT>* transition_action = new TRANS_ACTION();	
				connect<E_TYPE>( target, transition_action );
			}

			// добавить переход по условию
			template<class Predicate, class TRANS_ACTION> void connect(BaseState<CONTEXT> &target, const Predicate &guard) 
			{
				BaseTransitionAction<CONTEXT>* transition_action = new TRANS_ACTION();	
				connect<E_TYPE>( target, guard, transition_action );
			}


			friend class StateMachine<CONTEXT>;

	public:
		class BaseTransition {
		private:
			BaseState<CONTEXT> *source_;
			BaseState<CONTEXT> *target_;
		

			// ptr to class 
			BaseTransitionAction<CONTEXT>* transtion_action_;

		public:


			typedef void (CONTEXT::*ActionFun)();

			virtual TransitionType get_transition_type() const = 0;


			BaseTransition(BaseState<CONTEXT> *source, BaseState<CONTEXT> *target, BaseTransitionAction<CONTEXT>* transition_action) : 
			source_(source),
				target_(target), 
				transtion_action_(transition_action)
			{
				source_->outgoing_.push_back(this);
				target_->incoming_.push_back(this);
			}

			virtual ~BaseTransition() 
			{
				if (transtion_action_)
					delete transtion_action_;
			}

			virtual bool enabled(CONTEXT& context, const int event_id) = 0;

			/*
			inline ActionFun get_action_fun() 
			{
				return action_;
			}
			*/

			inline BaseTransitionAction<CONTEXT>* get_transition_action() 
			{
				return transtion_action_;
			}

			inline BaseTransitionAction<CONTEXT> const* get_transition_action() const
			{
				return transtion_action_;
			}

			inline BaseState<CONTEXT> * get_target()
			{
				return target_;
			}
		};

	protected:

		virtual bool is_initial() 
		{ 
			return false; 
		}

		virtual bool is_final() 
		{ 
			return false; 
		}

		virtual bool is_fork() 
		{ 
			return false; 
		}

	public:
		inline CONTEXT& context()
		{ 
			return *context_; 
		}

	protected:

		inline StateMachine<CONTEXT>& machine()
		{
			return *machine_;
		}

		// must be derived
		// return false if no need to log - e.i. not implemented
		// -------------------
		virtual bool entry_action(BaseEvent const& event) 
		{
			return false;
		}

		virtual bool exit_action(BaseEvent const& event)
		{
			return false;
		}

		// -------------------

		virtual bool can_activate() 
		{
			return true; 
		}

	public:
		vector<BaseTransition *> outgoing_;
		vector<BaseTransition *> incoming_;
	private:
		template<class Predicate>
		class GuardedTransition : public BaseTransition {

		public:
			virtual TransitionType get_transition_type() const
			{
				return TT_Guarded;
			}

			GuardedTransition(BaseState<CONTEXT> *source, BaseState<CONTEXT> *target, const Predicate &guard, BaseTransitionAction<CONTEXT>* transition_action) : 
				BaseTransition(source, target, transition_action), 
				guard_(guard) 
			{
			}

			virtual bool enabled(CONTEXT& context, const int event_id) 
			{
				return guard_(&context);
			}
		private:

			Predicate guard_;

		};


		class TriggeredTransition : public BaseTransition {
		public:

			virtual TransitionType get_transition_type() const
			{
				return TT_Triggered;
			}

			TriggeredTransition(BaseState<CONTEXT> *source, BaseState<CONTEXT> *target, const int event_id, BaseTransitionAction<CONTEXT>* transition_action) :
				BaseTransition(source, target,  transition_action), 
				event_id_(event_id) 
			{
			}


			virtual bool enabled(CONTEXT& context, const int event_id) 
			{
				return (event_id == event_id_);
			}
		private:
			//const type_info *event_id_;
			int const event_id_;

		};



		// формирует множество разрешенных переходов для следующего шага
		// если event_id == -1, может быть выбран только переход по условию
		void process_event(const int event_id, vector<BaseTransition*> &enabled) 
		{
			for(int i = 0; i < outgoing_.size();i++) {
				if (outgoing_[i]->enabled(context(), event_id)) {


					FSM_ASSERT(enabled.size() == 0 || is_fork());
					enabled.push_back(outgoing_[i]);
				}
			}
		}

	

	private:
		//String name_;

		CONTEXT *context_;

		// reference to machine
		StateMachine<CONTEXT>* machine_;

		vector<CppUtils::BaseAction<CONTEXT>*> entry_actions_;

		vector<CppUtils::BaseAction<CONTEXT>*> exit_actions_;

		

	};




	// ---------------------

	template<class CONTEXT>
	class BaseContext
	{

	public:
		// occures when we have fatal error
		virtual void on_fatal_error(
			StateMachineException const& e
		)
		{
				
		}


		// logging functions
		virtual void on_state_changed(
			BaseState<CONTEXT> const& exit_state, 
			BaseState<CONTEXT> const& target_state,  
			typename BaseState<CONTEXT>::TransitionType const trans_type,
			BaseEvent const& event_i
			)
		{
		}

		virtual void on_entry_action(
			BaseState<CONTEXT> const& target_state,
			BaseEvent const& event_i
			)
		{
		}

		virtual void on_exit_action(
			BaseState<CONTEXT> const& exit_state,
			BaseEvent const& event_i
			)
		{
		}

		virtual void on_entry_action_execute(
			BaseState<CONTEXT> const& target_state,
			BaseEvent const& event_i,
			BaseAction<CONTEXT> const& action
		)
		{
		}

		virtual void on_exit_action_execute(
			BaseState<CONTEXT> const& exit_state,
			BaseEvent const& event_i,
			BaseAction<CONTEXT> const& action
		)
		{
		}

		virtual void on_transition_action(BaseTransitionAction<CONTEXT> const& transition_action, BaseEvent const& eventToProcess)
		{
		}

		
		// happens when event is register in delayed queue
		virtual void on_delayed_event_register(BaseEvent const& event_i)
		{

		}

		virtual void on_direct_event_process(BaseEvent const& event_i)
		{

		}

		virtual void on_delayed_event_process(BaseEvent const& event_i)
		{

		}
		
		

		virtual ~BaseContext()
		{
		}


	};

	// --------------------------------



	template<class CONTEXT>
	class InitialState : public BaseState<CONTEXT> {
	public:

		CTOR(InitialState)

			virtual bool is_initial() 
		{ 
			return true; 
		}
	};

	// --------------------------------

	template<class CONTEXT>
	class FinalState : public BaseState<CONTEXT> {
	public:

		CTOR(FinalState)

			virtual bool is_final()
		{ 
			return true; 
		}
	};

	// --------------------------------

	template<class CONTEXT>
	class ForkState : public BaseState<CONTEXT> {
	public:

		CTOR(ForkState)

			virtual bool is_fork() 
		{ 
			return true; 
		}
	};

	// --------------------------------


	template<class CONTEXT>
	class JoinState : public BaseState<CONTEXT> {
		typename vector<BaseTransition*>::size_type num_joined_;

	public:
		CTOR(JoinState)


			JoinState():
		num_joined_(0)
		{
		}

	protected:
		virtual bool can_activate() {
			if (++num_joined_ >= incoming_.size()) {
				num_joined_ = 0;
				return true;
			}
			return false;
		}
	};

	// --------------------------------

	template<class CONTEXT>
	class StateMachine {

		typedef BaseState<CONTEXT> State;
		typedef BaseAction<CONTEXT> Action;

		friend class State;
		
	private:


		CONTEXT context_;
		// this is a mapped states i.e. mapped by class id. optional thing
		map<int, State*> mapped_states_;
		vector<State*> states_;
		vector<State*> active_states_;
		map<int, Action*>	mapped_actions_;
		
		// we accept recursion
		int reentry_counter_;

		// this is a special map to hold all recursive event to process later
		// it is not expected to use in multithread mode !!!
		queue<BaseEvent*> recursiveEvents_;
		// events to be sent for future events
		map<Uid, BaseEvent*> to_be_sent_;
		// this is based on the type events to be received
		map<int, UidSet > to_be_received_;

		bool fatal_exception_occured_;


	private:

		
		


		void post_event(BaseEvent& eventToProcess) {
			// entry
			reentry_counter_++;
			//LOG(MSG_INFO, "foo", "Counter increased to: " << reentry_counter_ << " event processed: " << eventToProcess.toString() );

			NullEvent null_event;
			// static NullEvent null_event; // DOES NOT WORK

			int ti = eventToProcess.getClassId();

			// before call callbacks to notify taht our event has sticked events 
			map<int, UidSet >::iterator s = to_be_received_.find(ti);
			if (s != to_be_received_.end()) {
				// found
				UidSet& sset = s->second;
				UidList to_delete;

				// propagate
				for(UidSet::const_iterator s2 = sset.begin(); s2 != sset.end(); s2++ ) {

					map<Uid, BaseEvent*>::const_iterator et = to_be_sent_.find(*s2);

					FSM_ASSERT(et != to_be_sent_.end());
					
					if (eventToProcess.mailed_event_arrived(*et->second)) {
						// need to remove this from emailing
						to_delete.push_back(*s2);

					}		
					
					
				}

				// remove if necessary
				for(int i = 0; i < to_delete.size(); i++) {
					to_be_sent_.erase(to_delete[i]);
					sset.erase(to_delete[i]);

					if (sset.size() == 0) {
						to_be_received_.erase( ti );
					}
				}

			}

			bool configuration_changed;
			do {
				vector<State*> new_active;
				configuration_changed = false;
				vector<State::BaseTransition*> transitions;

				for(int i = 0; i < active_states_.size(); i++) {

					transitions.clear();

					State& state_s =  *(active_states_[i]);
					state_s.process_event(ti, transitions);

					if (!transitions.empty()) {

						// execute set of exit actions
						for(int v = 0; v < state_s.exit_actions_.size(); v++) {
							BaseAction<CONTEXT>* ba = state_s.exit_actions_[v];
							if (ba->execute(eventToProcess))
								context_.on_exit_action_execute(state_s, eventToProcess, *ba);
						}

						if (state_s.exit_action(eventToProcess)) 
							// log if derived
							context_.on_exit_action(state_s, eventToProcess);
						

						for(int k = 0; k < transitions.size(); k++) {

							State::BaseTransition* base_transition = transitions[k];
						
							State & target_state = *base_transition->get_target();
						
														
							// transition action
							if (base_transition->get_transition_action() != NULL) {
								if (base_transition->get_transition_action()->execute(eventToProcess)) {
									context_.on_transition_action(*base_transition->get_transition_action(), eventToProcess);
								}
							}

							if (target_state.can_activate()) {
								
								if (target_state.getClassId() != state_s.getClassId()) {
								

									// execute set of entry actions
									for(int v = 0; v < target_state.entry_actions_.size(); v++) {
										BaseAction<CONTEXT>* ba = target_state.entry_actions_[v];
										if (ba->execute(eventToProcess))
											context_.on_entry_action_execute(target_state, eventToProcess, *ba);
									}

									if (target_state.entry_action(eventToProcess)) 
										// log if derived
										context_.on_entry_action(target_state, eventToProcess);
									
									

									//
									context_.on_state_changed(state_s, target_state, base_transition->get_transition_type(), ti >= 0 ? eventToProcess: null_event);
								}
								//else {
								//		LOG(MSG_INFO, "state_m_foo", "should not execute anithing target state: " << target_state.toString() << " as transition action happened");
								//}

								new_active.push_back(&target_state);
							}

						}	 // end iter through transitions

						configuration_changed = true;
					} else {
						new_active.push_back(&state_s);
					}
				}
				active_states_ = new_active;
				//event = NullEvent;
				ti = -1;
			} while (configuration_changed);

			// exit
			reentry_counter_--;
			//LOG(MSG_INFO, "foo", "Counter decreased to: " << reentry_counter_ << " event processed: " << eventToProcess.toString() );
			

		}


	public:

		// ctor
		StateMachine(void* contextPtr):
				context_(contextPtr),
					reentry_counter_(0),
					fatal_exception_occured_(false)
				{
				}

				// dtor - clear all
				~StateMachine() {

					to_be_received_.clear();

					for(map<Uid, BaseEvent*>::iterator s = to_be_sent_.begin(); s != to_be_sent_.end(); s++) {
						delete s->second;
					}

					to_be_sent_.clear();

					// clear recusrive events queue
					while(!recursiveEvents_.empty() ) {

							BaseEvent* p = recursiveEvents_.front();
							delete p;

							recursiveEvents_.pop();
					}
					

					for(int i = 0; i < states_.size(); i++) {
						delete states_[i];
					}

					states_.clear();
				}

				void init() {
					NullEvent nu;
					post_event(nu);
				}

			
				// parametrized version
				template<class S>
				S* add() {
					State *state = new S();
					add(state);
					
					return static_cast<S*>(state);
				}

				// usual version
				void add(State* state)
				{
					if (state->is_initial()) {
						FSM_ASSERT(active_states_.size() == 0);
						active_states_.push_back(state);
					}

					state->context_ = &context_;		
					state->machine_ = this;
					states_.push_back(state);

					mapped_states_[state->getClassId()] = state;
				}





				template<class E_TYPE>
				void do_full_process(BaseEvent& eventToProcess)
				{
					try {
						FSM_ASSERT(!fatal_exception_occured_);
						
						if (reentry_counter_ > 0) {
							// we are busy
							E_TYPE* eventCopy = new E_TYPE( (E_TYPE const&)eventToProcess );
							context_.on_delayed_event_register(eventToProcess);
							
							recursiveEvents_.push(eventCopy);
						} else if (reentry_counter_ == 0)	{
							
							// we are not busy
							context_.on_direct_event_process(eventToProcess);
							
							// process aht we have, all recursive events are registered in recursiveEvents_ 
							post_event(eventToProcess);

							FSM_ASSERT(reentry_counter_ == 0); 
							
							// process all what is left
						

							while(!recursiveEvents_.empty() ) {

								BaseEvent* p = recursiveEvents_.front();
								recursiveEvents_.pop();

								BaseEvent& event_i = *p;

								context_.on_delayed_event_process(event_i);

								// recursively call this
								post_event(event_i);

								
								delete p;

							}
						}
					 }	
					 catch(StateMachineException& e)
					 {
							fatal_exception_occured_ = true;
							context_.on_fatal_error(e);
					 }
					
				}

				// this register this event to be delivered should another event go later 
				// E_TYPE - to be sent, E_TARGET_TYPE- class receiving events
				template<class E_SEND_TYPE, class E_TARGET_TYPE>
				void register_event_to_recipient(BaseEvent const& sending_event)
				{
					// register
					to_be_sent_[sending_event.getEventUid()] = new E_SEND_TYPE( (E_SEND_TYPE const&)sending_event );

					int const event_id = E_TARGET_TYPE::CLASS_ID;
					map<int, UidSet >::iterator it = to_be_received_.find(event_id);
					if (it == to_be_received_.end()) {
						UidSet sset;
						sset.insert(sending_event.getEventUid());

						to_be_received_[event_id] = sset;
					}
					else {
						it->second.insert(sending_event.getEventUid());
					}

				}

				template<class E_TARGET_TYPE>
				void unregister_event_from_recipient(Uid const& sending_event_uid)
				{
					// unregister

					map<Uid, BaseEvent*>::iterator it = to_be_sent_.find(sending_event_uid);
					if (it != to_be_sent_.end()) {
						delete it->second;
						to_be_sent_.erase(sending_event_uid);
					}

					int const event_id = E_TARGET_TYPE::CLASS_ID;
					map<int, set<Uid> >::iterator it = to_be_received_.find(event_id);
					if (it != to_be_received_.end()) {
						it->second.erase(sending_event_uid);
					}

				}



				bool stopped() 
				{
					for (vector<State*>::iterator i = active_states_.begin(); i != active_states_.end(); i++) {
						if (!(*i)->is_final()) {
							return false;
						}
					}
					return true;
				}

				vector<State*> const& active_states() const 
				{ 
					return active_states_; 
				}

				inline CONTEXT& context()
				{ 
					return context_; 
				}

				State const* get_state_by_class_id(int const class_id) const
				{

					map<int, State*>::const_iterator it = mapped_states_.find(class_id);
					if (it == mapped_states_.end())
						return NULL;
					

					return it->second;
				}

				State* get_state_by_class_id(int const class_id)
				{

					map<int, State*>::iterator it = mapped_states_.find(class_id);
					if (it == mapped_states_.end())
						return NULL;

					return it->second;
				}

				template<class S>
				inline S const& state() const
				{
					State const* s= get_state_by_class_id(S::CLASS_ID);
					FSM_ASSERT(s != NULL);
					return static_cast<S const&>( *s );
				}

				template<class S>
				inline S& state()
				{
					State* s= get_state_by_class_id(S::CLASS_ID);
					FSM_ASSERT(s != NULL);
					return static_cast<S&>( *s );
				}

				// ----------------

				Action const* get_action_by_class_id(int const class_id) const
				{

					map<int, Action*>::const_iterator it = mapped_actions_.find(class_id);
					if (it == mapped_actions_.end())
						return NULL;
					

					return it->second;
				}

				Action* get_action_by_class_id(int const class_id)
				{

					map<int, Action*>::iterator it = mapped_actions_.find(class_id);
					if (it == mapped_actions_.end())
						return NULL;

					return it->second;
				}

				template<class A>
				inline A const& action() const
				{
					Action const* a= get_action_by_class_id(A::CLASS_ID);
					FSM_ASSERT(a != NULL);
					return static_cast<A const&>( *a );
				}

				template<class A>
				inline A& action()
				{
					Action* a= get_action_by_class_id(A::CLASS_ID);
					FSM_ASSERT(a != NULL);
					return static_cast<A&>( *a );
				}

	};

	/**
	* Helper to keep FSM list, assuming that we need to have a map of FSM
	*/

	template<class CONTEXT>
	class FSMMap: private CppUtils::Mutex
	{

	public:
		FSMMap()
		{
		}

		virtual ~FSMMap()
		{
			clear();
		}

		

		map<CppUtils::String, CppUtils::StateMachine<CONTEXT>* > & getMachineMap()
		{
			return machines_m;
		};

		map<CppUtils::String, CppUtils::StateMachine<CONTEXT>* > const& getMachineMap() const
		{
			return machines_m;
		};

		void clear()
		{
			LOCK_FOR_SCOPE(*this);

			for(map<CppUtils::String, CppUtils::StateMachine<CONTEXT>* >::iterator it = machines_m.begin(); it != machines_m.end();it++)
			{
				delete it->second;
			}

			machines_m.clear();

		};

		template<class E_TYPE>
		void processEvent(CppUtils::String const& symbol, CppUtils::BaseEvent& pev)
		{
			LOCK_FOR_SCOPE(*this);


			map<CppUtils::String, CppUtils::StateMachine<CONTEXT>* >::iterator it = machines_m.find(symbol);

			FSM_SOFT_ASSERT(it != machines_m.end());
			if (it != machines_m.end())	 {
				CppUtils::StateMachine<CONTEXT>& machine = *it->second;

				FSM_ASSERT(pev.getClassId() == E_TYPE::CLASS_ID ); 
				machine.do_full_process<E_TYPE>(pev);
			}
			
		};

		template<class E_TYPE, class E_TARGET_TYPE>
		void processEvent(CppUtils::String const& symbol, CppUtils::BaseEvent& pev)
		{
			LOCK_FOR_SCOPE(*this);
			map<CppUtils::String, CppUtils::StateMachine<CONTEXT>* >::iterator it = machines_m.find(symbol);

			FSM_SOFT_ASSERT(it != machines_m.end());
			if (it != machines_m.end())	 {
				CppUtils::StateMachine<CONTEXT>& machine = *it->second;

				FSM_ASSERT(pev.getClassId() == E_TYPE::CLASS_ID ); 

				machine.register_event_to_recipient<E_TYPE, E_TARGET_TYPE>(pev);
				machine.do_full_process<E_TYPE>(pev);
			}

			
		}

	private:

		map<CppUtils::String, CppUtils::StateMachine<CONTEXT>* > machines_m;
	};

	/**
	* Helper to work with a single machine
	*/
	template<class CONTEXT>
	class FSM: private CppUtils::Mutex
	{

	public:
		FSM():
			machine_m(NULL)
		{
		}

		virtual ~FSM()
		{
			clear();
		}

		void setMachine(CppUtils::StateMachine<CONTEXT>* ptr)
		{
			machine_m = ptr;
		}

		CppUtils::StateMachine<CONTEXT>& getMachine()
		{
			return *machine_m;
		};

		CppUtils::StateMachine<CONTEXT> const& getMachine() const
		{
			return *machine_m;
		};

		void clear()
		{
			LOCK_FOR_SCOPE(*this);

			if (machine_m) {
				delete machine_m;
				machine_m = NULL;
			}
		
		};

		template<class E_TYPE>
		void processEvent(CppUtils::BaseEvent& pev)
		{
			LOCK_FOR_SCOPE(*this);

			FSM_ASSERT(pev.getClassId() == E_TYPE::CLASS_ID ); 
			machine_m->do_full_process<E_TYPE>(pev);
			
		};

		template<class E_TYPE, class E_TARGET_TYPE>
		void processEvent(CppUtils::BaseEvent& pev)
		{
			LOCK_FOR_SCOPE(*this);
			
			FSM_ASSERT(pev.getClassId() == E_TYPE::CLASS_ID ); 

			machine_m->register_event_to_recipient<E_TYPE, E_TARGET_TYPE>(pev);
			machine_m->do_full_process<E_TYPE>(pev);
			
		}

	private:

		CppUtils::StateMachine<CONTEXT>* machine_m;
	};

}; // end of namespace




#endif //#ifndef HSM_H__