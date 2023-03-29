#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#include <cstdio>
#include <string>
#include <vector>

//for backgound thread
#include <mutex>
#include <pthread.h>
#include <condition_variable>
#include <thread>
#include <deque>

#include <liburing.h>
#include <map>

#define DEBUG

#ifdef DEBUG
#define LOG(fmt, ...)                                               \
  do {                                                              \
    fprintf(stderr, "%s:%d:" fmt, __FILE__, __LINE__, __VA_ARGS__); \
  } while (0)

#define LOG2(fmt, ...)                 \
  do {                                 \
    fprintf(stderr, fmt, __VA_ARGS__); \
  } while (0)
#endif

//void BackGroundManager();

class BPlusTree {
  struct Meta;
  struct Monitor;
  struct Index;
  struct Record;
  struct Per_page_Log;
  struct Node;
  struct IndexNode;
  struct LeafNode;
  struct Plog;
  class BlockCache;
  class InternalCache;
  struct LogRecord;
  struct Checkpoint;
  
        

 public:
  BPlusTree(const char* path, const char* log_path, const char* chpt_path, const int bg_id_, const bool enable_backthd);
  ~BPlusTree();

  void SetIOStaic();
  off_t GetChpt();
  void SetLoadFlag(int flag);
  void DisableFLUOrNot(int flag);
  void LoadIOStatic(int &plog_insert, int &leaf_insert, int &leaf_split,int &phy_plog_write,int &phy_plog_read,int &phy_page_read,int &phy_page_write,int &logi_page_read,int &logi_page_write);
  void SetPlog();
  void SetBaseline();
  void GetStatic(int &hits_bp, int &hits_glog, int &hits_plog, int &disk_read, int &io_consume, int &error_cnt,int &dirtys, int &cleans);
  void ResetStatic();
  off_t GetLSN();
  void IncreLSN();

  
  void Put(const std::string& key, const std::string& value, off_t lsn);
  void InstantRecovery(off_t checkpoint);
  void NormalRecovery(off64_t checkpoint);
  void ApplyOneDiskToDataFile(int seg_id);
  void BackGround_GLog_Func();
  int ApplyPendPages(std::map<off_t, std::pair<char*,char*>>* source, std::map<off_t, std::pair<char*,char*>>* target, std::map<off_t,std::pair<int,int>> *EmptyPages);
  void AIOFLushPlog_Demand(struct io_uring *ring);
  void InitBGGlog();
 
  bool Delete(const std::string& key);
  bool Get(const std::string& key, std::string& value) const;
  std::vector<std::pair<std::string, std::string>> GetRange(
      const std::string& left_key, const std::string& right_key) const;
  bool Empty() const;
  size_t Size() const;

#ifdef DEBUG
  void Dump();
#endif

 //private:

  template <typename T>
  T* AllocInMemPage()const;

  template <typename T>
  T* Map(off_t offset, int isleaf, int is_plog, char* &plog_copy) const; //is_plog: 1:leafnode; 1:plog; 3: Get both leaf and plog copy by one time  

 
  
  template <typename T>
  T* MapIndex(int page_id) const;

  template <typename T>
  T* MapLeaf(int page_id) const;

  
  template <typename T>
  void InsertDirty(off_t offset) const;
  template <typename T>
  void RelocateDirty(off_t offset_leaf, off_t offset_split) const;

  template <typename T>
  void DirectInsertDirty(off_t offset, off_t ori_oldset_lsn) const;

  
  template <typename T>
  void UnMap(T* map_obj, int isleaf, int is_plog) const;
  template <typename T>
  T* AllocLeaf();
  template <typename T>
  T* AllocIndex();
  template <typename T>
  void Dealloc(T* node);

  size_t GetMinKeys(int flag);
  size_t GetMaxKeys(int flag);
  size_t GetMinIndexKeys();
  size_t GetMaxIndexKeys();

  template <typename T>
  int UpperBound(T arr[], int n, const char* target, int flag) const;
  template <typename T>
  int UpperBoundIndex(T arr[], int n, const char* target) const;
  template <typename T>
  int LowerBound(T arr[], int n, const char* target, int flag) const;
  template <typename T>
  int LowerBoundIndex(T arr[], int n, const char* target) const;

  off_t GetLeafOffset(const char* key) const;
  void WarmUpVirtualPageCache(int x) const;
  LeafNode* SplitLeafNode(LeafNode* leaf_node);
  IndexNode* SplitIndexNode(IndexNode* index_node);
  size_t InsertKeyIntoIndexNode(IndexNode* index_node, const char* key,
                                Node* left_node, Node* right_node,off_t cur_lsn);
  size_t InsertKVIntoLeafNode(LeafNode* leaf_node, const char* key,
                              const char* value, off_t cur_lsn);
  size_t InsertKVIntoPlog(Plog* plog, const char* key,
                              const char* value, off_t cur_lsn);
  int GetIndexFromLeafNode(LeafNode* leaf_node, const char* key) const;
  int GetIndexFromPlog(Plog* plog, const char* key) const;
  IndexNode* GetOrCreateParent(Node* node);

  bool BorrowFromLeftLeafSibling(LeafNode* leaf_node);
  bool BorrowFromRightLeafSibling(LeafNode* leaf_node);
  bool BorrowFromLeafSibling(LeafNode* leaf_node);
  bool MergeLeftLeaf(LeafNode* leaf_node);
  bool MergeRightLeaf(LeafNode* leaf_node);
  LeafNode* MergeLeaf(LeafNode* leaf_node);

  bool BorrowFromLeftIndexSibling(IndexNode* index_node);
  bool BorrowFromRightIndexSibling(IndexNode* index_node);
  bool BorrowFromIndexSibling(IndexNode* index_node);
  bool MergeLeftIndex(IndexNode* index_node);
  bool MergeRightIndex(IndexNode* index_node);
  IndexNode* MergeIndex(IndexNode* index_node);

  int Check(off_t offset);

  char* GetMemCopy(off_t offset);
  Monitor* GetMtr();
  

  int fd_;
  int log_fd_;
  int chpt_fd_;
  int bg_id;
  BlockCache* block_cache_;
  InternalCache* internal_cache_;
  Meta* meta_;
  Monitor* mtr_;
  LogRecord* log_obj;
  Checkpoint* chpt;
  std::thread BackGround_Glog;
};

#endif  // BPLUS_TREE_H