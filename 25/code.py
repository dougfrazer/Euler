a,b,count = 0,1,0
while(b < 10**1000):
  a,b,count = b,a+b, count+1
print(count)
