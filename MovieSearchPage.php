<html>
<h1> MOVIE Information!</h1>
<body>
	<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
		<input type="text" size="80" name="search">
		<input type="Submit" value="Search Actors!">
	</form>
<?php
	if ($_SERVER["REQUEST_METHOD"] == "GET"){
		
		$name = $_GET['search'];
		$words = explode(' ', $name);
		echo "$words[0]<br>";
		echo "$words[1]<br>";
		$database = new mysqli('localhost', 'cs143', '', 'CS143');
    	if($database->connect_errno > 0){
   	    	 die('Unable to connect to database [' . $database->connect_error . ']');
   		}
   		//"kyle haacker"
   		$i=0;
   		$query = "SELECT id, title, year FROM Movie";
   		echo count($words);
   		while($i < count($words)){
   			echo "$words[$i] is in row $i<br>";
   			if($i == 0){
   				$query = $query . " WHERE";
   			}
   			else{
   				$query = $query . " and";
   			}
   		   $query = $query . " (title like '%$words[$i]%')";
   		//$query = "SELECT * FROM Actor order by last";
   		//$rStatement = $database->prepare("SELECT id, first, last, dob FROM Actor where last like ? or first like ?;");
	       $i++;
        }
        $query = $query . ";";
    	$result = $database->query($query);
        //$rStatement->bind_param("ss", $name, $name);
        //$rStatement->execute();
        //$rStatement->bind_result($r_id, $r_first, $r_last, $r_dob);


        echo "<table border='1' bordercolor='black' cellpadding='2'>";
        echo "<tr>";
        echo "<th>Title</th>";
        echo "<th>Year</th>";
        echo "</tr>";
        echo "hello";
        //while($rStatement->fetch()){
        $i = 0;
        while($row = $result->fetch_array(MYSQLI_ASSOC)){
        	echo "$i";
        	$i++;
        ///fix 
        	$title = $row['title'];
        	$year = $row['year'];
        	$id = $row['id'];
        	


        	echo "<tr>";
        	echo "<td> <a href=\"MovieLinkResult.php?id=".$row["id"]."\">".$title."</a></td>";
        	echo "<td> <a href=\"MovieLinkResult.php?id=".$row["id"]."\">".$year."</a> </td>";
        	echo "</tr>";
        }
        echo "</table>";



	}
?>



</body>
</html>