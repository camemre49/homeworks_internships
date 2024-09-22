#
# Utility functions
#
def rcgetter(p):
    pr = len(p)
    pc = len(p[0])
    return pr, pc


def pattern90converter(patt):
    row, column = rcgetter(patt)
    convertedlist = []
    convertedcols = ""
    for j in range(column):
        for i in range(row-1, -1, -1):  # Indexes of the rows from bottom to top.
            convertedcols += patt[i][j]
        convertedlist.append(convertedcols)
        convertedcols = ""
    return convertedlist


def occurencefinder(currentstr, length, searchedstr):
    instances = []
    for i in range(len(currentstr) - length + 1):
        if currentstr[i:i + length] == searchedstr:
            instances += [i]
    return instances


def searcher(image, pattern):
    possiblestarts = []
    for i in range(len(image)):  # rows of the image
        currentrow = image[i]
        occurences = occurencefinder(currentrow, len(pattern[0]), pattern[0])
        if occurences:
            for j in occurences:  # Because it is list of occurence indexes.
                possiblestarts += [(i, j)]
    return possiblestarts


def completesearch(possiblestars, image, pattern):
    patternrows, patterncols = rcgetter(pattern)
    imagerows, imagecols = rcgetter(image)
    if imagecols < patterncols or imagerows < patternrows:
        return False
    for i in possiblestars:
        consistents = 1  # Because first row is already matched while finding possible starts.
        startingrow, startingcolumn = i[0], i[1]
        patternindex = 1
        if patternrows == 1:
            return i
        for j in range(startingrow + 1, startingrow + patternrows):
            if startingrow + patternrows <= len(image):  # Whether the pattern can fit by starting that point
                if pattern[patternindex] == image[j][startingcolumn: startingcolumn + patterncols]:
                    consistents += 1
                patternindex += 1
            if consistents == patternrows:
                return i
    return 0


#
# Main function to search pattern in an image
# 
def pattern_search(inputpattern, inputimage):
    for x in range(4):
        theta = x*90
        result = completesearch(searcher(inputimage, inputpattern), inputimage, inputpattern)
        if type(result) == tuple:
            return result[0], result[1], theta
        inputpattern = pattern90converter(inputpattern)
    return False

#
# An example for test
#
# originalimage = ["tuz<abcd", ">#sAY#at", "uzyXAAr.", "r,lAXxio", "z#a!yabc", "yazy?zya"]
# originalpattern = ["AXA", "XAY"]
# print(pattern_search(originalpattern, originalimage))