module PE3 where

data Expression = Variable String 
    | Constant Float
    | Sine Expression
    | Cosine Expression
    | Negation Expression
    | Addition Expression Expression
    | Multiplication Expression Expression

class Differential a where
    diff :: a -> a -> a

instance Show Expression where
    show (Variable x) = "Variable " ++ "'" ++ x ++ "'"
    show (Constant c) = "Constant " ++ show c
    show (Sine expr) = "sin " ++ show expr
    show (Cosine expr) = "cos " ++ show expr
    show (Negation expr) = "-" ++ show expr
    show (Addition expr1 expr2) = "(" ++ show expr1 ++ " + " ++ show expr2 ++ ")"
    show (Multiplication expr1 expr2) = "(" ++ show expr1 ++ " * " ++ show expr2 ++ ")"

instance Eq Expression where
    (==) (Variable x1) (Variable x2)  = x1 == x2
    (==) (Constant f1) (Constant f2) = f1 == f2
    (==) (Sine expr1) (Sine expr2) = expr1 == expr2
    (==) (Cosine expr1) (Cosine expr2) = expr1 == expr2
    (==) (Negation expr1) (Negation expr2) = expr1 == expr2
    (==) (Addition expr11 expr12) (Addition expr21 expr22) = expr11 == expr21 && expr12 == expr22
    (==) (Multiplication expr11 expr12) (Multiplication expr21 expr22) = expr11 == expr21 && expr12 == expr22
    (==) _ _ = False

instance Num Expression where
    fromInteger a = Constant (fromInteger a)
    negate (Constant a) = Constant (-a)
    negate expr = (Negation expr)
    (*) expr1 expr2 = Multiplication expr1 expr2
    (+) expr1 expr2 = Addition expr1 expr2
    abs _ = Constant 0
    signum _ = Constant 0


instance Differential Expression where
    diff (Constant _) _ = Constant 0

    diff (Variable x) (Variable dv) 
        | (Variable x) ==  (Variable dv) = Constant 1
        | otherwise = Constant 0
    
    diff (Sine expr) (Variable dv) = Multiplication (Cosine expr) (diff expr (Variable dv))

    diff (Cosine expr) (Variable dv) = Multiplication (Negation (Sine expr)) (diff expr (Variable dv))

    diff (Negation expr) (Variable dv) = Negation (diff expr (Variable dv))

    diff (Addition expr1 expr2) (Variable dv) = Addition (diff expr1 (Variable dv)) (diff expr2 (Variable dv))
    
    diff (Multiplication expr1 expr2) (Variable dv) =
        Addition (Multiplication (diff expr1 (Variable dv)) expr2) (Multiplication expr1 (diff expr2 (Variable dv)))
        

precedence :: String -> Int
precedence "-" = 4  -- Unary minus
precedence "sin" = 3  -- Sine function
precedence "cos" = 3  -- Cosine function
precedence "*" = 2  -- Multiplication
precedence "+" = 1  -- Addition
precedence "(" = 0  -- Opening parenthesis
precedence ")" = 0  -- Closing parenthesis
precedence _ = error "Unknown operator or function"  -- Error for unsupported operators/functions


isOperator :: String -> Bool
isOperator "+" = True
isOperator "-" = True
isOperator "*" = True
isOperator "/" = True
isOperator "sin" = True
isOperator "cos" = True
isOperator _ = False

isParenOpen :: String -> Bool
isParenOpen "(" = True
isParenOpen _ = False

isParenClose :: String -> Bool
isParenClose ")" = True
isParenClose _ = False

isEmpty :: [a] -> Bool
isEmpty [] = True
isEmpty _  = False

clearStackUntilPrecedence :: String -> [String] -> [String] -> ([String], [String])  
clearStackUntilPrecedence operator stack addQueue
    | isEmpty stack = ((stack ++ [operator]), addQueue)
    | (precedence (last stack)) < precedence operator = ((stack ++ [operator]), addQueue)
    | (isParenClose operator) && ((precedence (last stack)) == precedence operator) = ((init stack), addQueue)
    | ((precedence (last stack)) == precedence operator) = (((init stack) ++ [operator]), addQueue ++ [(last stack)])
    | otherwise = clearStackUntilPrecedence operator (init stack) (addQueue ++ [(last stack)])




shuntingyard :: [String] -> [String] -> [String] -> [String]
shuntingyard expression stack queue  
    | isEmpty expression = queue ++ (reverse stack) -- Return statement
    | isParenOpen (head expression) = shuntingyard (tail expression) (stack ++ [(head expression)]) queue
    | isParenClose (head expression) = shuntingyard (tail expression) (fst (clearStackUntilPrecedence ")" stack [])) (queue ++ (snd (clearStackUntilPrecedence ")" stack [])))
    | (isEmpty stack) && (isOperator (head expression)) = shuntingyard (tail expression) (stack ++ [(head expression)]) queue
    | isOperator (head expression) = shuntingyard (tail expression) (fst (clearStackUntilPrecedence (head expression) stack [])) (queue ++ (snd (clearStackUntilPrecedence (head expression) stack [])))
    | otherwise = shuntingyard (tail expression) stack (queue  ++ [(head expression)])
