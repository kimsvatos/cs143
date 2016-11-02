<html>
  <head>
    <title>
      CS143 Movie Database
    </title>
    <style>
      body {
          margin: 0;
      }
      ul {
          list-style-type: none;
          margin: 0;
          padding: 0;
          width: 25%;
          background-color: #A9A9A9;
          position: fixed;
          height: 100%;
          overflow: auto;
      }

      li a {
          display: block;
          color: #000;
          padding: 8px 16px;
          text-decoration: none;
          font-family: "Arial", sans-serif;
      }

      li a.home {
          padding: 8px 8px;
      }

      li a.active {
          background-color: #0080FF;
          color: black;
      }

      li a.header {
          background-color: #000000;
          color: white;
          padding: 8px 8px;
      }

      li a:hover:not(.active, .header) {
          background-color: #66B3FF;
          color: black;
      }
    </style>
  </head>
  <body>
    <ul>
      <li><a class="home" href="./index.php">Home</a></li>
      <li><a class="header">Add Content</a></li>
      <li><a href="./test.php">Add New Actor/Director</a></li>
      <li><a href="./movieInfo.php">Add New Movie</a></li>
      <li><a href="./movieActor.php">Add Movie/Actor Relationship</a></li>
      <li><a href="./movieDirector.php">Add Movie/Director Relationship</a></li>
      <li><a href="./review.php">Add Review</a></li>
      <li><a class="header">Search Content</a></li>
      <li><a class="active" href="./ActorInfo.php">Search Actors</a></li>
      <li><a href="./MovieSearchPage.php">Search Movies</a></li>
      <li><a href="./GenSearch.php">Search All</a></li>
    </ul>
    <div style="margin-left:25%; padding:1px 16px; height:1000px;">

    <h1>Search Actors</h1>

	<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
		<input type="text" size="80" name="search">
		<input type="Submit" value="Search Actors!">
	</form>
<?php
	if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {
		
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
   		$query = "SELECT id, first, last, dob FROM Actor";
   		echo count($words);
   		while($i < count($words)){
   			echo "$words[$i] is in row $i<br>";
   			if($i == 0){
   				$query = $query . " WHERE";
   			}
   			else{
   				$query = $query . " and";
   			}
   		   $query = $query . " (last like '%$words[$i]%' or first like '%$words[$i]%')";
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
        echo "<th>Actor Name</th>";
        echo "<th>Date of Birth</th>";
        echo "</tr>";
        echo "hello";
        //while($rStatement->fetch()){
        $i = 0;
        while($row = $result->fetch_array(MYSQLI_ASSOC)){
        	echo "$i";
        	$i++;
        ///fix 
        	$name = $row['first'] . " " . $row['last'];
        	$dob = $row['dob'];
        	$id = $row['id'];
        	


        	echo "<tr>";
        	echo "<td> <a href=\"ActorLinkResult.php?id=".$row["id"]."\">".$name."</a></td>";
        	echo "<td> <a href=\"ActorLinkResult.php?id=".$row["id"]."\">".$dob."</a> </td>";
        	echo "</tr>";
        }
        echo "</table>";



	}
?>

</div>
</body>
</html>