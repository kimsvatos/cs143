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
    $id_query = "SELECT first, last, dob, sex, dod FROM Actor WHERE id=$id;";
    $queryRes = $database->query($id_query);
    $result = $queryRes->fetch_array(MYSQLI_ASSOC);
    echo $result['last'];
    echo " <h1> Actor Information is: </h1>";
    echo "<table border='1' bordercolor='black' cellpadding='2'>";
    echo "<tr>";
    echo "<th>Actor Name</th>";
    echo "<th>Sex</th>";
    echo "<th>Date of Birth</th>";
    echo "<th>Date of Death</th>";
    echo "</tr>";

    //add info below
    echo "<tr>";
    echo "<td>" . $result['first'] . " " . $result['last'] . "</td>";
	echo "<td>" . $result['sex'] . "</td>";
	echo "<td>" . $result['dob'] . "</td>";
	if($result['dod'] == NULL){
		echo "<td> N/A </td>";
	}
	else{
		echo "<td>" . $result['dod'] . "</td>";
	}
	echo "</tr>";
	echo "</table>";


	///next table
    echo "<h1> Actor's Movies and Role:</h1>";
   // echo "</table>";
?>
	<form method="post" action="ActorInfo.php">
        <input type="Submit" value="Go back to make another search!">
    </form>

</body>


</html>