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
      <li><a href="./GenSearch.php">Search All</a></li>
    </ul>

    <div style="margin-left:25%; padding:1px 16px; height:auto;">
    <div id="banner">
      <div id="banner-content"><h1>Actor Information</h1>
      </div>
    </div>

    <div id="main-body">

<!-- do queries and show table with DOB name DOD blah --> 
<?php
//if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {
	$id = $_GET['id'];
	$database = new mysqli('localhost', 'cs143', '', 'CS143');
    if($database->connect_errno > 0){
        die('Unable to connect to database [' . $database->connect_error . ']');
    }

  
    $id_query = "SELECT first, last, dob, sex, dod FROM Actor WHERE id=$id;";
    $queryRes = $database->query($id_query);
    $result = $queryRes->fetch_array(MYSQLI_ASSOC);

    echo "<table>";
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

	//movie info 
	$id_query = "SELECT role, id, title from MovieActor, Movie WHERE aid = $id AND mid=id";
    $queryRes = $database->query($id_query);
    
	///next table
    echo "<h1> Actor's Movies and Roles:</h1>";
    echo "<table>";
    echo "<tr>";
    echo "<th>Movie Title</th>";
    echo "<th>Role</th>";
   
    echo "</tr>";

    while($result = $queryRes->fetch_array(MYSQLI_ASSOC)){
    echo "<tr>";
    echo "<td> <a href=\"MovieLinkResult.php?id=".$result["id"]."\">".$result['title']."</a></td>";
    echo "<td>".$result['role']."</a></td>";
	echo "</tr>";
	}	

	echo "</table>";

//}
?>
	<form method="post" action="ActorInfo.php">
        <br>
        <input type="Submit" value="Search Another Actor!">
    </form>
</div>
</div>
</body>


</html>