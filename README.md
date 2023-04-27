# Regular-expression-checker

Regex:
```
(((a)∗)(b))
((b)(a))
```
Input:
```
abbaababaaabbaa
```
Output: 
```
$1$2$1$1$1$2@a#
```
**Example:**

Content of input.txt:
```
3

((((a)(a))+)(b))

(((b)(((a)(a))*))(b))

((((a)(a))*)(b))

baaaabaabba
```
Content of output.txt: 
```$2$1$3@a#```

**Example:**

Content of input.txt:
```
3

((((b)|((a)(b)))*)|((((b)|((a)(b)))*)(a)))

((((a)(a))+)(b))

(((b)(((a)(a))*))(b))

baaaabaabbabaa
```
Content of output.txt: 
```
$3$2$1$1#
```

