<html>
<body>
<h1>Kim And Kyle's Calcultor!!!!11!1</h1>
<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
  <input type="text" name="nums">
  <input type="submit" value="Calculate">
</form>

<?php
if ($_SERVER["REQUEST_METHOD"] == "GET") {
    // collect value of input field
    $eq = $_GET['nums'];
    if (empty($eq)) {
        
    } else {
        
        $eq = str_replace(" ", "", $eq);
        $subject = $eq;
        $pattern = "/[^0123456789+-\/\.\*]/";
        $zeroCheck = strpos($eq, "/0");
        if($zeroCheck){
            echo "<h2>Result</h2>";       
            exit("Division by zero error!");
        }
       preg_match($pattern, $subject, $matches);

       if(empty($matches)){ 
              eval("\$result = $eq;");
              $pattern = "/[+-\/\*\.][+\/\*\.]/";
              preg_match($pattern, $subject, $matches);
              if(empty($matches)){
                   echo "<h2>Result</h2>";
                   echo $eq . " = " . $result; 
              }
              else{
                  echo "<h2>Result</h2>";
                  echo "Invalid Expression!";
              }
       
        }
        else{
        echo "<h2>Result</h2>";
        echo "Invalid Expression!";
        }
    }
}
?>

</body>
</html>



