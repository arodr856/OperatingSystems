All problems are included in file structure and are self describing file names.

The following is for #10:

One can see that ParThread is the fastest, followed by ParFork, then MyCompress.

./MyCompress data.txt results.txt  0.00s user 0.00s system 53% cpu 0.008 total

./ParFork data.txt results.txt 4  0.00s user 0.00s system 102% cpu 0.007 total

./ParThread data.txt results.txt 4  0.00s user 0.00s system 68% cpu 0.005 total

