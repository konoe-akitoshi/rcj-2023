with open("test.txt", mode = "w") as f:
    f.write("1 2")

with open("test.txt", mode = "r") as f:
    data = f.read()

print(data)
