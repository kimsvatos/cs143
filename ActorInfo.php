<html>
<h1> Actor Information!</h1>
<body>
	<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
		<input type="text" size="80" name="search">
		<input type="Submit" value="Search Actors!">
	</form>
<?php
	if ($_SERVER["REQUEST_METHOD"] == "GET"){
		
		$name = $_GET['search'];
		$name = "%" . $name . "%";

		$database = new mysqli('localhost', 'cs143', '', 'CS143');
    	if($database->connect_errno > 0){
   	    	 die('Unable to connect to database [' . $database->connect_error . ']');
   		}
   		//"kyle haacker"
   		$query = "SELECT id, first, last, dob FROM Actor where last like $name or first like $name;";
        $result = $database->query($query);
        echo "<table border='1' bordercolor='black' cellpadding='2'>";
        echo "<tr>";
        echo "<th>Actor Name</th>";
        echo "<th>Date of Birth</th>";
        echo "</tr>";
        while($iter= $result->fetch_array(MYSQLI_ASSOC)){
        	echo "hi";
        	$name = $iter['first'] . " " . $iter['last'];
        	$dob = $iter['dob'];
        	$id = $iter['id'];

        	echo "<tr>";
        	echo "<td> $name </td>";
        	echo "<td> $dob </td>";
        	echo "</tr>";
        }
        echo "</table>";



	}
?>



</body>
</html>