

#ifndef _CPPUTILS_LANGDEFS_INCLUDED
#define _CPPUTILS_LANGDEFS_INCLUDED



//=============================================================
// Error strings in this package:
//
//
// exc_Configuration_FailedToCreateRootKey
//
// Configuration: Raised when class Configuration cannot create 
// the root registry key, usually due to lacking privileges of the user 
// running the app.
//
//
// exc_SharedResoruce_Deadlock
//
// Internal: Raised by class SharedResource when a thread
// deadlock is detected.
//
//
// exc_PythonWrapper_CannotResolveSymbol
//
// Configuration: Raised when class PyWrapper cannot resolve 
// expected symbols in the dynamically loaded Python DLL
//
//
// exc_PythonWrapper_CannotImportTraceback
//
// Configuration: Raised when class PyWrapper cannot import
// the Python module "traceback"
//
//
// exc_MalformedSignature
//
// Internal: Raised when class Signature encounters an invalid 
// hex-representation while attempting to destringify the signature
//
//
// exc_TransactionManager_CannotRegisterDuringTransaction
//
// Internal: Raised when an attempt is made to register a transactional
// object while the manager is in a transaction.
//
//
// exc_TransactionManager_InvalidContext
//
// Raised when a client refers to a transaction that is unknown; this
// may happen when a transaction has timed out
//
//
// exc_TransactionManager_DeadlockDetected
//
// Internal: Raised when a deadlock in requested transactions is detected
//
//
// exc_TransactionManager_CannotNestWriteInReadTransaction
//
// Internal: Raised when a client request to start a write transaction
// as sub-transaction of a top-level read-only transaction.
//
//
// exc_TransactionManager_FailedToStartTransaction
//
// Internal: Raised when starting a transaction fails because one of the 
// transactional objects subject to the transaction reported an error.
//
//
// exc_TransactionManager_FailedToEndReadOnlyTransaction
//
// Internal: Raised when ending a read transaction produces an error of
// one of the transactional objects subject to the transaction.
//
//
// exc_TransactionManager_FailedToCommitInconsistent
//
// Internal: Raised when committing a transaction failed and the resulting 
// state is known to be inconsistent.
//
//
// exc_TransactionManager_FailedToCommitConsistent
//
// Internal: Raised when committing a transaction failed and the resulting 
// state is known to be consistent.
//
//
// exc_TransactionManager_FailedToAbortInconsistent
//
// Internal: Raised when aborting a transaction failed and the resulting 
// state is known to be inconsistent.







#endif // _CPPUTILS_LANGDEFS_INCLUDED
