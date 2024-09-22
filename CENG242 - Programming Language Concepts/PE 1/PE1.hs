module PE1 where

-- Question 1
findInList :: (Eq a) => [a] -> [a] -> [Int]
findInList patternList listToBeSearched = findIn patternList listToBeSearched 0

findIn :: (Eq a) => [a] -> [a] -> Int -> [Int]
findIn [] _ _ = []
findIn patternList listToBeSearched index = offset : findIn (tail patternList) rest (offset + 1)
                                          where offset = fst (findOccurrences (head patternList) listToBeSearched index)
                                                rest = snd ((findOccurrences (head patternList) listToBeSearched index))

findOccurrences :: (Eq a) => a -> [a] -> Int -> (Int, [a])
findOccurrences target (x:xs) index
    | x == target = (index, xs)
    | otherwise = findOccurrences target xs (index + 1)

    

-- Question 2
findInMatrix :: (Eq a) => [[a]] -> [[a]] -> [[(Int, Int)]]
findInMatrix [] _ = []
findInMatrix patternList listToBeSearched = makeMatrix (length patternList) (length (head patternList)) (findAsList patternList listToBeSearched)

makeMatrix :: Int -> Int -> [a] -> [[a]]
makeMatrix row col flatList = chunk col flatList
    where
        chunk _ [] = []
        chunk elementCount list = take elementCount list : chunk elementCount (drop elementCount list)


findAsList :: (Eq a) => [[a]] -> [[a]] -> [(Int, Int)]
findAsList patternList listToBeSearched = findInFlooredMatrix (flattenMatrix [] [] patternList) (diagonalMatrix listToBeSearched)


diagonalMatrix :: [[a]] -> [((Int, Int), a)]
diagonalMatrix matrix = flattenMatrix [] [] (matrixIndices matrix 0)

-- [((0,0),1),((0,1),2),((1,0),1),((0,2),3),((1,1),2),((2,0),5),((0,3),4),((1,2),3),((2,1),6),((1,3),4),((2,2),7),((2,3),8)]
findInFlooredMatrix :: Eq a => [a] -> [((Int, Int), a)] -> [(Int, Int)]
findInFlooredMatrix [] _ = []
findInFlooredMatrix patternList listToBeSearched = value : findInFlooredMatrix (tail patternList) rest
                                           where value = fst (findInFloor (head patternList) listToBeSearched)
                                                 rest  = snd (findInFloor (head patternList) listToBeSearched)
                                               



findInFloor :: Eq b => b -> [((Int, Int), b)] -> ((Int, Int), [((Int, Int), b)])
findInFloor target (x:xs)
    | target == snd x = (fst x, xs)
    | otherwise = findInFloor target xs
    

matrixIndices :: [[a]] -> Int -> [[((Int, Int), a)]]
matrixIndices [] _ = []
matrixIndices (first:rest) index = indexElements first index : matrixIndices rest (index + 1)


-- Function to traverse the matrix and create an array of tuples with indices and elements
indexElements ::  [a] -> Int -> [((Int, Int), a)]
indexElements list index = [((index, i), list !! i) | i <- [0..((length list) - 1)]]


flattenMatrix []  []           []       = []      
flattenMatrix xss []           (zs:zss) =     flattenMatrix []       (reverse $ zs:xss) zss
flattenMatrix xss ((y:ys):yss) zss      = y : flattenMatrix (ys:xss) yss                zss  
flattenMatrix xss ([]    :yss) zss      =     flattenMatrix xss      yss                zss
flattenMatrix xss []           []       =     flattenMatrix []       (reverse xss)      []
