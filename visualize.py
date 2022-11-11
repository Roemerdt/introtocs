from matplotlib import pyplot
import os
import numpy

FILENAME = "a.out"
INPUT = "1.in"

pyplot.rc('axes', axisbelow=True)

output = os.popen(f"cat {INPUT} | ./{FILENAME}").read().split(";")
new_list = []
for i in range(len(output)):
  temp = output[i].split(",")
  numbers = [int(i) for i in temp]
  new_list.append(numbers)
data = numpy.array(new_list)
x,y = data.T
fig,ax = pyplot.subplots()

ax.spines['left'].set_position('center')
ax.spines['bottom'].set_position('center')
ax.spines['right'].set_color('none')
ax.spines['top'].set_color('none')
ax.xaxis.set_ticks_position('bottom')
ax.yaxis.set_ticks_position('left')

ax.plot(x,y, "ok")
ax.plot(x,y)
count = 1
for xitem,yitem in numpy.nditer([x,y]):
        etiqueta = count
        pyplot.annotate(etiqueta, (xitem,yitem), textcoords="offset points",xytext=(0,10),ha="center")
        count += 1
pyplot.axis("on")
pyplot.show()
print(output)