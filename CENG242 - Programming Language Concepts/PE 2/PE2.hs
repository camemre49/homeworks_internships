module PE2 where

data Tree k v = EmptyTree | Node k v [Tree k v] deriving (Show, Eq)

-- Question 1
selectiveMap :: (a -> Bool) -> (a -> a) -> [a] -> [a]
selectiveMap _ _ [] = []
selectiveMap filterF mapF (first : rest) = if (filterF first) then (mapF first) : selectiveMap filterF mapF rest
                                           else first : (selectiveMap filterF mapF rest)

-- Question 2
tSelectiveMap :: (k -> Bool) -> (v -> v) -> Tree k v -> Tree k v
tSelectiveMap _ _ EmptyTree = EmptyTree  
tSelectiveMap filterF mapF (Node key value children) = if filterF key then Node key (mapF value) applyChildren
                                                       else Node key value applyChildren
                                                       where applyChildren = (map (tSelectiveMap filterF mapF) children)

-- Question 3
tSelectiveMappingFold :: (k -> Bool) -> (k -> v -> r) -> (r -> r -> r) -> r -> Tree k v -> r
tSelectiveMappingFold _ _ _ acc EmptyTree = acc
tSelectiveMappingFold filterF mapF combineF accumulator (Node key value children) =
    let parentValue = if filterF key then combineF accumulator (mapF key value) else accumulator
    in foldLeft ( tSelectiveMappingFold filterF mapF combineF ) parentValue children

foldLeft :: (a -> b -> a) -> a -> [b] -> a
foldLeft func s [] = s
foldLeft func s (el : rest) = foldLeft func (func s el) rest

-- Question 4
searchTree :: (Eq v, Eq k) => v -> Tree k v -> (k -> v)
searchTree def = tSelectiveMappingFold a b c d
    where a = (\key -> True)     
          b = (\key value -> (\oldVal -> if key==oldVal then value else def))
          c = combineFunctions 
                    where 
                        combineFunctions f g = \x -> f x `combine` g x
                            where 
                                combine a b = if a == def then b else a 
          d = (\searchParam -> def)



