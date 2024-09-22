def FunctionFlatter(functionList):  # Remove the spaces
    flattenList = []
    for function in functionList:
        newFunction = ""
        for character in function:
            if character == " " or character == "\n" or character == "\r":
                continue
            else:
                newFunction += character
        flattenList.append(newFunction)
    return flattenList


def FunctionForest(flatList):  # Make all functions' trees.
    functionForest = []
    functionIndexes = []
    for function in flatList:
        operation = leftChild = rightChild = ""
        name = function[0:4]
        functionIndexes += name[0]
        function = function[5:]
        for i in range(len(function)):
            if function[i] == "+" or function[i] == "-" or function[i] == "*" or function[i] == "^":
                operation = function[i]
                leftChild = function[:i]
                rightChild = function[i + 1:]
                break
        functionTree = [name, operation, [leftChild], [rightChild]]
        functionForest.append(functionTree)
    return functionForest, functionIndexes


def makeRealtree(functionTree):
    if functionTree == "x" or functionTree[0] in ["0", "1", "2", "3", "4", "5", "6", "7", "8", "9"]:
        return [functionTree]
    leftChild = functionTree[2][0]
    rightChild = functionTree[3][0]
    isInFunction = 0
    if len(leftChild) > 1 and leftChild[1] == "(":  # if it is a function
        leftName = leftChild[0]
        isInFunction = 1
        index = functionIndexes.index(leftName)
        leftChild = functionForest[index]
        isUsed[index] += 1
    if len(rightChild) > 1 and rightChild[1] == "(":  # if it is a function
        rightName = rightChild[0]
        isInFunction = 1
        index = functionIndexes.index(rightName)
        rightChild = functionForest[index]
        isUsed[index] += 1
    if not isInFunction:
        return functionTree
    else:
        return [functionTree[0], functionTree[1], makeRealtree(leftChild), makeRealtree(rightChild)]


def RealTrees(definitions):  # Swap function names with real trees in function definitions.
    global functionForest, functionIndexes
    functionForest, functionIndexes = FunctionForest(FunctionFlatter(definitions))
    global isUsed
    isUsed = [0 for i in range(len(functionIndexes))]
    realTrees = []
    for functionTree in functionForest:
        realTrees += [makeRealtree(functionTree)]
    used = []
    for tree in realTrees:
        name = tree[0][0]
        if isUsed[functionIndexes.index(name)] > 0:
            used.append(tree)
    for i in used:
        realTrees.remove(i)

    return realTrees


def construct_forest(definitions):
    result = ""  # I forgot to remove (x) :)
    forest = RealTrees(definitions)
    forest = str(forest)
    i = 0
    while i < len(forest):
        if forest[i] == "(":
            i += 3
            continue
        result += forest[i]
        i += 1
    result = eval(result)
    return result