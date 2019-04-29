# Test ability to handle combination of fetches and requests
serve s1 s2 s3
generate random-text1.txt 2K 
generate random-text2.txt 4K 
generate random-text3.txt 6K
generate random-text4.txt 8K
generate random-text5.txt 10K
generate random-text6.txt 12K
fetch f1 random-text1.txt s1
request r1 random-text1.txt s2
fetch f2 random-text2.txt s2
request r2 random-text2.txt s3
fetch f3 random-text3.txt s3
request r3 random-text3.txt s1
fetch f4 random-text4.txt s3
request r4 random-text4.txt s1
fetch f5 random-text5.txt s2
request r5 random-text5.txt s3
fetch f6 random-text6.txt s1
request r6 random-text6.txt s2
wait *
check f1
check f2
check f3
check f4
check f5
check f6
respond r4 r5 r6
wait *
respond r1 r2 r3
wait *
check r1
check r2
check r3
check r4
check r5
check r6
quit

