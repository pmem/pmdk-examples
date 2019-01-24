/*
  Copyright (c) 2004, 2010, Oracle and/or its affiliates

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

/** @file ha_pmdk.h

    @brief
  The ha_pmdk engine is a stubbed storage engine for pmdk purposes only;
  it does nothing at this point. Its purpose is to provide a source
  code illustration of how to begin writing new storage engines; see also
  /storage/pmdk/ha_pmdk.cc.

    @note
  Please read ha_pmdk.cc before reading this file.
  Reminder: The pmdk storage engine implements all methods that are *required*
  to be implemented. For a full list of all methods that you can implement, see
  handler.h.

   @see
  /sql/handler.h and /storage/pmdk/ha_pmdk.cc
*/

#ifdef USE_PRAGMA_INTERFACE
#pragma interface			/* gcc class implementation */
#endif

#include "my_global.h"                   /* ulonglong */
#include "thr_lock.h"                    /* THR_LOCK, THR_LOCK_DATA */
#include "handler.h"                     /* handler */
#include "my_base.h"                     /* ha_rows */
#include <libpmemobj.h>
#include <libpmemobj++/p.hpp>
#include <libpmemobj++/persistent_ptr.hpp>
#include <libpmemobj++/make_persistent.hpp>
#include <map>
#include <unordered_map>
#include <string>
#include <stdio.h>
#include <stdlib.h>


using namespace pmem::obj;

POBJ_LAYOUT_BEGIN(mysql_obj);
POBJ_LAYOUT_TOID(mysql_obj, struct table_row);
POBJ_LAYOUT_TOID(mysql_obj, struct row);
POBJ_LAYOUT_END(mysql_obj);

typedef std::multimap<const std::string, persistent_ptr<row> >::iterator rowItr;

struct table_row {
  uchar buf[PMEMOBJ_MIN_POOL];
};
struct row_args {
  uchar *buf;
  size_t len;
};

struct row {
  persistent_ptr<row> next;
  uchar buf[];
};

struct root {
  persistent_ptr<row> rows;
};

/** @brief
  pmdk_share is a class that will be shared among all open handlers.
  This pmdk implements the minimum of what you will probably need.
*/
class pmdk_share : public Handler_share {
public:
  mysql_mutex_t mutex;
  THR_LOCK lock;
  pmdk_share();
  ~pmdk_share()
  {
    thr_lock_delete(&lock);
    mysql_mutex_destroy(&mutex);
  }
};

/** @brief
  Class definition for the storage engine
*/
class ha_pmdk: public handler
{
  THR_LOCK_DATA lock;      ///< MySQL lock
  pmdk_share *share;    ///< Shared lock info
  pmdk_share *get_share(); ///< Get the share

private:
  PMEMobjpool *objtab;
  std::map<int,int> errCodeMap;
  persistent_ptr<row> iter;
  persistent_ptr<row> current;
  persistent_ptr<row> prev;
  persistent_ptr<root> proot;

public:
  ha_pmdk(handlerton *hton, TABLE_SHARE *table_arg);
  ~ha_pmdk()
  {
    if (!errCodeMap.empty())
      errCodeMap.clear();
  }

  /** @brief
    The name of the index type that will be used for display.
    Don't implement this method unless you really have indexes.
   */
  const char *index_type(uint inx) { return "HASH"; }

  /** @brief
    This is a list of flags that indicate what functionality the storage engine
    implements. The current table flags are documented in handler.h
  */
  ulonglong table_flags() const
  {
    /*
      We are saying that this engine is just statement capable to have
      an engine that can only handle statement-based logging. This is
      used in testing.
    */
    return HA_BINLOG_STMT_CAPABLE;
  }

  /** @brief
    This is a bitmap of flags that indicates how the storage engine
    implements indexes. The current index flags are documented in
    handler.h. If you do not implement indexes, just return zero here.

      @details
    part is the key part to check. First key part is 0.
    If all_parts is set, MySQL wants to know the flags for the combined
    index, up to and including 'part'.
  */
  ulong index_flags(uint inx, uint part, bool all_parts) const
  {
    ulong flags = HA_READ_NEXT | HA_READ_PREV | HA_READ_ORDER
                  | HA_READ_RANGE | HA_KEYREAD_ONLY;
    return(flags);

  }

    /** @brief
    unireg.cc will call max_supported_record_length(), max_supported_keys(),
    max_supported_key_parts(), uint max_supported_key_length()
    to make sure that the storage engine can handle the data it is about to
    send. Return *real* limits of your storage engine here; MySQL will do
    min(your_limits, MySQL_limits) automatically.
   */
  uint max_supported_record_length() const { return HA_MAX_REC_LENGTH; }

  /** @brief
    unireg.cc will call this to make sure that the storage engine can handle
    the data it is about to send. Return *real* limits of your storage engine
    here; MySQL will do min(your_limits, MySQL_limits) automatically.

      @details
    There is no need to implement ..._key_... methods if your engine doesn't
    support indexes.
   */
  uint max_supported_keys()          const { return 10; }

  /** @brief
    unireg.cc will call this to make sure that the storage engine can handle
    the data it is about to send. Return *real* limits of your storage engine
    here; MySQL will do min(your_limits, MySQL_limits) automatically.

      @details
    There is no need to implement ..._key_... methods if your engine doesn't
    support indexes.
   */
  uint max_supported_key_parts()     const { return 10; }

  /** @brief
    unireg.cc will call this to make sure that the storage engine can handle
    the data it is about to send. Return *real* limits of your storage engine
    here; MySQL will do min(your_limits, MySQL_limits) automatically.

      @details
    There is no need to implement ..._key_... methods if your engine doesn't
    support indexes.
   */
  uint max_supported_key_length()    const { return 128; }

  /** @brief
    Called in test_quick_select to determine if indexes should be used.
  */
  virtual double scan_time() { return (double) (stats.records+stats.deleted) / 20.0+10; }

  /** @brief
    This method will never be called if you do not implement indexes.
  */
  virtual double read_time(uint, uint, ha_rows rows)
  { return (double) rows /  20.0+1; }

  /*
    Everything below are methods that we implement in ha_pmdk.cc.

    Most of these methods are not obligatory, skip them and
    MySQL will treat them as not implemented
  */
  /** @brief
    We implement this in ha_pmdk.cc; it's a required method.
  */
  int open(const char *name, int mode, uint test_if_locked);    // required

  /** @brief
    We implement this in ha_pmdk.cc; it's a required method.
  */
  int close(void);                                              // required

  /** @brief
    We implement this in ha_pmdk.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int write_row(uchar *buf);

  /** @brief
    We implement this in ha_pmdk.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int update_row(const uchar *old_data, const uchar *new_data);

  /** @brief
    We implement this in ha_pmdk.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int delete_row(const uchar *buf);

  /** @brief
    We implement this in ha_pmdk.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_read_map(uchar *buf, const uchar *key,
                     key_part_map keypart_map, enum ha_rkey_function find_flag);

  /** @brief
    We implement this in ha_pmdk.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_next(uchar *buf);

  /** @brief
    We implement this in ha_pmdk.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_prev(uchar *buf);

  /** @brief
    We implement this in ha_pmdk.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_first(uchar *buf);

  /** @brief
    We implement this in ha_pmdk.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_last(uchar *buf);

  int index_init(uint index, bool sorted);

  int index_end();

  int index_read(
                uchar*                  buf,
                const uchar*            key,
                uint                    key_len,
                ha_rkey_function        find_flag);

  /** @brief
    Unlike index_init(), rnd_init() can be called two consecutive times
    without rnd_end() in between (it only makes sense if scan=1). In this
    case, the second call should prepare for the new table scan (e.g if
    rnd_init() allocates the cursor, the second call should position the
    cursor to the start of the table; no need to deallocate and allocate
    it again. This is a required method.
  */
  int rnd_init(bool scan);                                      //required
  int rnd_end();
  int rnd_next(uchar *buf);                                     ///< required
  int rnd_pos(uchar *buf, uchar *pos);                          ///< required
  void position(const uchar *record);                           ///< required
  int info(uint);                                               ///< required
  int extra(enum ha_extra_function operation);
  int external_lock(THD *thd, int lock_type);                   ///< required
  int delete_all_rows(void);
  ha_rows records_in_range(uint inx, key_range *min_key,
                           key_range *max_key);
  int delete_table(const char *from);
  int create(const char *name, TABLE *form,
             HA_CREATE_INFO *create_info);                      ///< required

  THR_LOCK_DATA **store_lock(THD *thd, THR_LOCK_DATA **to,
                             enum thr_lock_type lock_type);     ///< required
  void populate_errcodemap(void);
  void insertRowIntoIndexTable(Field *field, std::string key_, persistent_ptr<row> row);
  int start_stmt(THD *thd, thr_lock_type lock_type);
  int deleteNodeFromSLL();
  bool searchNode(const persistent_ptr<row> &rowPtr);
  std::string IdentifyTypeAndConvertToString(const uchar*, int,int len=0,int offset=0);
  void deleteRowFromAllIndexedColumns(const persistent_ptr<row> &row);
  bool isPrimaryKey(void);
  void loadIndexTableFromPersistentMemory(void);
};


class key
{
   public:
      int insert(const std::string keyValue, persistent_ptr<row> row);
      bool updateRow(const std::string oldStr, const std::string newStr);
      void deleteRow(const persistent_ptr<row> &row);
      void deleteALLRows();
      void setMapPosition(rowItr iter);
      rowItr getFirst();
      rowItr getCurrent();
      rowItr getLast();
      bool verifyKey(const std::string key);
      void backupForRollBack();
      void rollBackMap();
      void clearRollbackMap();
   private:
      std::multimap<const std::string, persistent_ptr<row> > rows;
      std::multimap<const std::string, persistent_ptr<row> > rollbackMap;
      rowItr mapPosition;
};

class table_
{
   public:
      bool getKeys(const char* columnName, key **p);
      int insert(const char* columnName, key*);
      bool deleteKey(const char* columnName);
      bool checkColumnsForRollBack();	
      bool clearTransactionTempData();	
   private:
      std::unordered_map<const char*, key*> keys;
};

class database
{
   public:
      static database* getInstance();
      bool getTable(const char* tableName,table_ **t);
      int insert(const char* tableName, table_*);
      bool deleteTable(const char* TableName);
   private:
      database(){}
      database(const database &){}
      std::unordered_map<const char*, table_*> tables;
      static database *m_db;
};
