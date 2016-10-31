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
    $id_query = "SELECT first, last, dob, dod FROM Actor WHERE id=$id;";
    $queryRes = $database->query($id_query);
    $result = $queryRes->fetch_array(MYSQLI_ASSOC);
    echo "$result[0]";
?>

</body>


</html>