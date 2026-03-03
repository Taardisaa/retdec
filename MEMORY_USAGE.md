Summary of amplification
Component	Per-item overhead	Scale in libavcodec	Estimated total
DU/UD chains	O(stmts × vars) per func	~10K functions	dominant — multi-GB
ValueAnalysis cache	~600-1000B per entry	100K-1M entries	600MB-1GB
CFGs (rebuilt per optimizer)	~100KB per function	~10K functions	~1GB
Statement nodes + ShPtr control blocks	~120B + ~48B control block	100K-1M stmts	160MB-1.6GB
Expression tree nodes	~50-100B each	500K-5M exprs	250MB-500MB
