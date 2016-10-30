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
		$words = explode(' ', $name);
		

		$database = new mysqli('localhost', 'cs143', '', 'CS143');
    	if($database->connect_errno > 0){
   	    	 die('Unable to connect to database [' . $database->connect_error . ']');
   		}
   		//"kyle haacker"
   		$i=0;
   		$query = "SELECT id, first, last, dob FROM Actor";
   		while($i < count($words)){
   			if($i == 0){
   				$query = $query . " WHERE";
   			}
   			else{
   				$query = $query . " and";
   			}
   		$query = $query . " (last like '%$words[i]%' or first like '%$words[i]%')";
   		//$query = "SELECT * FROM Actor order by last";
   		//$rStatement = $database->prepare("SELECT id, first, last, dob FROM Actor where last like ? or first like ?;");
        $i = $i + 1;
        }
        $query = $query . ";";
    	$result = $database->query($query);
        //$rStatement->bind_param("ss", $name, $name);
        //$rStatement->execute();
        //$rStatement->bind_result($r_id, $r_first, $r_last, $r_dob);


        echo "<table border='1' bordercolor='black' cellpadding='2'>";
        echo "<tr>";
        echo "<th>Actor Name</th>";
        echo "<th>Date of Birth</th>";
        echo "</tr>";
        echo "hello";
        //while($rStatement->fetch()){
        while($row = $result->fetch_array(MYSQLI_ASSOC)){}
        	echo "hi";

        ///fix 
        	$name = $row['first'] . " " . $row['last'];
        	$dob = $row['dob'];
        	$id = $row['id'];

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