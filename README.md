# pB+-tree Project
It is a project trying to solve the logging dilemma for modern B+-tree indexing built atop the novel storage device with transparent compression. 

# Interface
  With the BPlusTree object, you can invoke PUT(), GET(), etc interface for evaluation.

# Features
  1. Utilize liburing + typical file system interface (e.g. write(), read()) for block IO.
  2. Each BPlusTree object is a full-featured pB+-Tree implementation, which involves LRU+FLU buffer pool, ARIES logging component, Async IO component, PUT()/GET() interfaces, etc.
  3. Support multi-client processing, see test.cc template for details.

# Benchmark
  YCSB benckmark with Random, Uniform, ScrambledZipf, SkewedZipf distributions for generating key-values. It is embedded into test.cc.
  
# Build
  * liburing must be installed for block IO, which functionalities have been integrated into the implementation.
  * Configurations, including: 
  
      (1) (dis)enable baseline (typical B+-tree implementaion); 
      
      (2) (dis)enable pB+-tree (pB+-tree implementaion);
      
      (3) parameters (e.g., buffer pool size, key-value size, buffer pool thresholds, workload-related settings) should be tuned manually in test.cc
      
      (4) Makefile should be updated based on your config

```
make && ./test
```

