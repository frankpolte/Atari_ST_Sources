rem sine table maker in 1st basic

n=&hc0000
values=170
count=0

for t=0 to (3.1415927*2) step (3.1415927*2)/values

offset=int(91+(sin(t)*91))
count=count+2

pokew n,offset
n=n+2
next t

bsave "a:\trk_y.bin",&hc0000,count