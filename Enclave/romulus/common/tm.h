/**
 * Modified version of the original file from romulus
 * Peterson Yuhala
 */


/* Definitions for different Persistent Transactional Memories */


#ifdef ROMULUS_LOG_PTM
#include "romuluslog/RomulusLog.hpp"
#define TM_WRITE_TRANSACTION   romuluslog::RomulusLog::write_transaction
#define TM_READ_TRANSACTION    romuluslog::RomulusLog::read_transaction
#define TM_BEGIN_TRANSACTION() romuluslog::gRomLog.begin_transaction()
#define TM_END_TRANSACTION()   romuluslog::gRomLog.end_transaction()
#define TM_ALLOC               romuluslog::RomulusLog::alloc
#define TM_FREE                romuluslog::RomulusLog::free
#define TM_PMALLOC             romuluslog::RomulusLog::pmalloc
#define TM_PFREE               romuluslog::RomulusLog::pfree
#define TM_TYPE                romuluslog::persist
#define TM_NAME                romuluslog::RomulusLog::className
#define TM_CONSISTENCY_CHECK   romuluslog::RomulusLog::consistency_check
#define TM_INIT                romuluslog::RomulusLog::init
#endif



#ifdef NOTHING_STM
#include "NothingTM.hpp"
#define TM_WRITE_TRANSACTION   nothingtm::write_transaction
#define TM_READ_TRANSACTION    nothingtm::read_transaction
#define TM_BEGIN_TRANSACTION   nothingtm::begin_transaction
#define TM_END_TRANSACTION     nothingtm::end_transaction
#define TM_ALLOC               nothingtm::alloc
#define TM_FREE                nothingtm::free
#define TM_PMALLOC             nothingtm::pmalloc
#define TM_PFREE               nothingtm::pfree
#define TM_TYPE                nothingtm::tmtype
#define TM_NAME                nothingtm::NothingTM::className
#define TM_CONSISTENCY_CHECK   nothingtm::NothingTM::consistency_check
#endif
