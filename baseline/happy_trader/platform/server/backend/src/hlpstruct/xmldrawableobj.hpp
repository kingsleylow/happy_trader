#ifndef _HLPSTRUCT_XMLDRAWABLEOBJ_INCLUDED
#define _HLPSTRUCT_XMLDRAWABLEOBJ_INCLUDED


#include "hlpstructdefs.hpp"
#include "pricedata.hpp"
#include "xmlparameter.hpp"


namespace HlpStruct {

	
	struct HLPSTRUCT_EXP RtDrawableObj {

		friend HLPSTRUCT_EXP void convertDrawableObjToXmlParameter(RtDrawableObj const& rtdrawableobj, XmlParameter &xmlparameter);
		
		friend HLPSTRUCT_EXP void convertDrawableObjFromXmlParameter(RtDrawableObj &rtdrawableobj, XmlParameter const &xmlparameter_const);



	public:

		enum DOType {
			DO_DUMMY =			0,
			DO_LINE_INDIC = 1,
			DO_PRINT =			2,
			DO_OHLC =				3,
			DO_TEXT =				4,
			DO_LEVEL1 =			5
		};

		static const int MAX_SHORT_TEXT_LENGTH = 31;

		static const int MAX_LONG_TEXT_LENGTH = 8191;

		// nested struct
		struct LineIndicator {
			LineIndicator():
			color_m(-1),
			value_m(-1)
			{
			}

			LineIndicator(CppUtils::String const& name, int const color, double const& value):
			color_m(color),
			value_m(value),
			name_m(name)
			{
			}

			LineIndicator(char const* name, int const color, double const& value):
			color_m(color),
			value_m(value),
			name_m(name != NULL ? name : "")
			{
			}

			void clear()
			{
				color_m = -1;
				value_m = -1;
				name_m.clear();

			}


			int color_m;
			double value_m;
			CppUtils::String name_m;
		};

		struct Print {
			
			Print():
				bid_m(-1),
				ask_m(-1),
				volume_m(-1),
				color_m(-1),
				operation_m(-1)
			{
			}

			void clear()
			{
				bid_m = -1;
				ask_m = -1;
				volume_m = -1;
				color_m = -1;
				operation_m = -1;

			}

			double bid_m;
			double ask_m;
			double volume_m;
			int color_m; 
			int operation_m;
		};

		struct OHLC
		{
			OHLC():
				open_m(-1),
				high_m(-1),
				low_m(-1),
				close_m(-1),
				volume_m(-1)
			{
			}
			
			void clear()
			{
				open_m = -1;
				high_m = -1;
				low_m = -1;
				close_m = -1;
				volume_m = -1;
			}

			double open_m;
			double high_m;
			double low_m;
			double close_m;
			double volume_m;
		};

		struct Text {

      
			Text()
			{
				clear();
			}

			inline void clear()
			{
				memset(this, '\0', sizeof(Text));
			}

			inline void setShortText(char const* shorttext)
			{
				if (shorttext) {
					shortText_m[0] = '\0';
					strncat(shortText_m, shorttext, MAX_SHORT_TEXT_LENGTH);
				}
			}

			inline void setShortText(CppUtils::String const& shorttext)
			{
				shortText_m[0] = '\0';
				strncat(shortText_m, shorttext.c_str(), MAX_SHORT_TEXT_LENGTH);

			}

			inline void setText(char const* text)
			{
				if (text) {
					text_m[0] = '\0';
					strncat (text_m, text, MAX_LONG_TEXT_LENGTH );
				}
			}

			inline void setTextWrappedToCDataTags(char const* text)
			{
				
				text_m[0] = '\0';
				strcat(text_m, "<![CDATA[" );
				  
				if (text) {
					strncat (text_m, text, MAX_LONG_TEXT_LENGTH - 9 - 3 );
				}
				
				strcat(text_m, "]]>");
			
			}


			int priority_m;
			
			char shortText_m[MAX_SHORT_TEXT_LENGTH+1];

			char text_m[MAX_LONG_TEXT_LENGTH+1];

		};

		struct Level1
		{
			Level1():
				best_bid_price_m(-1),
				best_ask_price_m(-1),
				best_bid_volume_m(-1),
				best_ask_volume_m(-1)
			{
			}
			
			void clear()
			{
				best_bid_price_m = -1;
				best_ask_price_m = -1;
				best_bid_volume_m = -1;
				best_ask_volume_m = -1;
			}

			double best_bid_price_m;
			double best_ask_price_m;
			double best_bid_volume_m;
			double best_ask_volume_m;

		};

	public:
		RtDrawableObj():
			time_m(-1),
			type_m(DO_DUMMY)
		{
		}

		// accessors
		inline double& getTime()
		{
			return time_m;
		}

		inline double const& getTime() const
		{
			return time_m;
		}

		void setTime(double const timeval)
		{
			time_m = timeval;
		}

		inline CppUtils::String const& getSymbol() const
		{
			return symbol_m;
		}

		inline CppUtils::String const& getProvider() const
		{
			return provider_m;
		}

		inline CppUtils::String& getSymbol()
		{
			return symbol_m;
		}

		inline CppUtils::String& getProvider()
		{
			return provider_m;
		}

		inline CppUtils::String& getName()
		{
			return name_m;
		}

		inline CppUtils::String const& getName() const
		{
			return name_m;
		}

		inline CppUtils::String& getRunName()
		{
			return runName_m;
		}

		inline CppUtils::String const& getRunName() const
		{
			return runName_m;
		}
			
		// accessors
		// ----------

		inline vector<LineIndicator> const& getAsLineIndicator() const
		{
			return (vector<LineIndicator> const&)lineIndicList_m;
		}

		inline vector<LineIndicator>& getAsLineIndicator()
		{
			return lineIndicList_m;
		}

		// ----------

		inline Print const& getAsPrint() const
		{
			return (Print const&)print_m;
		}

		inline Print& getAsPrint()
		{
			return print_m;
		}

		// ----------
		inline OHLC const& getAsOHLC() const
		{
			return (OHLC const&)ohcl_m;
		}

		inline OHLC& getAsOHLC()
		{
			return ohcl_m;
		}

		// ----------
		inline Text const& getAsText() const
		{
			return (Text const&)text_m;
		}

		inline Text& getAsText()
		{
			return text_m;
		}

		// ----------
		inline Level1 const& getAsLevel1() const
		{
			return (Level1 const&)level1_m;
		}

		inline Level1& getAsLevel1()
		{
			return level1_m;
		}

		// ----------

		inline DOType const& getType() const
		{
			return type_m;
		}

		inline DOType& getType()
		{
			return type_m;
		}
		
		
		void clear();

	
	private:

		double time_m;

		DOType type_m;

		CppUtils::String name_m;

		CppUtils::String provider_m;

		CppUtils::String symbol_m;

		CppUtils::String runName_m;

		// data
		vector<LineIndicator> lineIndicList_m;
		
		Print print_m;
		
		OHLC ohcl_m;

		Text text_m;

		Level1 level1_m;

	};

};

#endif