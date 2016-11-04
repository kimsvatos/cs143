<html>
  <head>
    <title>
      CS143 Movie Database
    </title>
    <link rel=StyleSheet HREF="./style.css" TYPE="text/css" MEDIA="screen">
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
      <li><a href="./ActorInfo.php">Search Actors</a></li>
      <li><a href="./MovieSearchPage.php">Search Movies</a></li>
      <li><a class="active" href="./GenSearch.php">Search All</a></li>
    </ul>

    <div style="margin-left:25%; padding:1px 16px; height:auto;">
    <div id="banner">
      <div id="banner-content"><h1>Search All</h1>
      </div>
    </div>

    <div id="main-body">

	<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
		<input type="text" size="80" maxlength="200" name="search" placeholder="Max 200 characters"><br><br>
		<input type="Submit" value="Search All!">
	</form>
<?php
if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {
		

		$name = $_GET['search'];
		$words = explode(' ', $name);

		$database = new mysqli('localhost', 'cs143', '', 'CS143');
    	if($database->connect_errno > 0){
   	    	 die('Unable to connect to database [' . $database->connect_error . ']');
   		}
   		//"kyle haacker"
   		$i=0;
   		$query = "SELECT id, title, year FROM Movie";

   		while($i < count($words)){

   			if($i == 0){
   				$query = $query . " WHERE";
   			}
   			else{
   				$query = $query . " and";
   			}
   		   $query = $query . " (title like '%$words[$i]%')";

	       $i++;
        }
        $query = $query . ";";
      	$result = $database->query($query);


		    echo "<h1>Matching Movies Are:</h1>";
        echo "<table>";
        echo "<tr>";
        echo "<th>Title</th>";
        echo "<th>Year</th>";
        echo "</tr>";

        $i = 0;
        while($row = $result->fetch_array(MYSQLI_ASSOC)){
        
        	$i++;

        	$title = $row['title'];
        	$year = $row['year'];
        	$id = $row['id'];
        	
        	echo "<tr>";
        	echo "<td> <a href=\"MovieLinkResult.php?id=".$row["id"]."\">".$title."</a></td>";
        	echo "<td> <a href=\"MovieLinkResult.php?id=".$row["id"]."\">".$year."</a> </td>";
        	echo "</tr>";
        }
        echo "</table>";

        //SECOND SEARCH!! ACTOR!
        echo "<br><br>";
        $i=0;
   		$query = "SELECT id, concat(first, ' ', last) fullName, dob FROM Actor";

   		while($i < count($words)){

   			if($i == 0){
   				$query = $query . " WHERE";
   			}
   			else{
   				$query = $query . " and";
   			}
   		   $query = $query . " (last like '%$words[$i]%' OR first like '$words[$i]%')";

	       $i++;
        }
        $query = $query . ";";
    	$result = $database->query($query);

    	echo "<h1>Matching Actors Are:</h1>";
    	echo "<table>";
        echo "<tr>";
        echo "<th>Name</th>";
        echo "<th>Date of Birth</th>";
        echo "</tr>";


        $i = 0;
        while($row = $result->fetch_array(MYSQLI_ASSOC)){

        	$i++;

        	$fullName = $row['fullName'];
        	$dob = $row['dob'];
        	$id = $row['id'];
        	
        	echo "<tr>";
        	echo "<td> <a href=\"ActorLinkResult.php?id=".$row["id"]."\">".$fullName."</a></td>";
        	echo "<td> <a href=\"ActorLinkResult.php?id=".$row["id"]."\">".$dob."</a> </td>";
        	echo "</tr>";
        }
        echo "</table>";

	}
?>

</div>
</div>
</body>
</html>