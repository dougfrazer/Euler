a,b,count = 1,1,2
while(b < 10**999):
  a,b,count = b,a+b, count+1
print(count)
