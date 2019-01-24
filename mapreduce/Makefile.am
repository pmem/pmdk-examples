AUTOMAKE_OPTIONS = foreign
bin_PROGRAMS = wordcount
wordcount_SOURCES = wordcount.cpp helpers.cpp list_entry.cpp persistent_string.cpp pm_mapreduce.cpp task_list.cpp
wordcount_CPPFLAGS = $(PMEM_CFLAGS) $(PMEMOBJ_CFLAGS) $(PMEMOBJPP_CFLAGS)
wordcount_LDFLAGS = $(PMEM_LIBS) $(PMEMOBJ_LIBS) $(PMEMOBJPP_LIBS)
