<html>
<body>


<h1>Information</h1>
<!-- do queries and show table with DOB name DOD blah --> 
<?php
	$id = $_GET['id'];
	$database = new mysqli('localhost', 'cs143', '', 'CS143');
    if($database->connect_errno > 0){
        die('Unable to connect to database [' . $database->connect_error . ']');
    }

    echo "$id";
?>

</body>


</html>