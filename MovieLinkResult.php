<!DOCTYPE html>
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
      <div id="banner-content">
        <h1>Movie Information</h1>
      </div>
    </div>

    <div id="main-body">

<?php
//if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {

	$id = $_GET['id'];
	$database = new mysqli('localhost', 'cs143', '', 'CS143');
    if($database->connect_errno > 0){
        die('Unable to connect to database [' . $database->connect_error . ']');
    }

  
    $id_query = "SELECT title, dob, company, year, rating, concat(first,' ',last) directorName, genre FROM Movie m LEFT OUTER JOIN MovieGenre mg ON m.id = mg.mid LEFT OUTER JOIN MovieDirector md ON m.id = md.mid LEFT OUTER JOIN Director d ON d.id = md.did WHERE m.id = $id;";
    $queryRes = $database->query($id_query);
    while($result = $queryRes->fetch_array(MYSQLI_ASSOC)){
        $title = $result['title'];
        $year = $result['year'];
        $dir = $result['directorName'];
        $dob = $result['dob'];
        $comp = $result['company'];
        $rating = $result['rating'];
        $genre = $genre . " " . $result['genre'];
    }


    echo "<b>Title</b>: " . $title . " (" . $year . ")<br>";
    echo "<b>Company</b>: " . $comp . "<br>";
    echo "<b>Rating</b>: " . $rating . "<br>";
    if($dir == "0" || $dir == NULL){
        echo "<b>Director</b>: <br>";
    }
    else{
        echo "<b>Director</b>: " . $dir . " (" . $dob . ")<br>";
    }
    
    echo "<b>Genre</b>: " . $genre . "<br>";
    echo "<h1> Actors in this movie: </h1>";
    echo "<table>";
    echo "<tr>";
    echo "<th>Actor Name</th>";
    echo "<th>Role</th>";
    echo "</tr>";

    $id_query = "SELECT CONCAT(a.first,' ',a.last) actorName, a.id res, ma.role FROM Actor a, Movie m, MovieActor ma WHERE m.id = $id and ma.mid = m.id and ma.aid = a.id;";
    $queryRes = $database->query($id_query);
    while($result = $queryRes->fetch_array(MYSQLI_ASSOC)){
        echo "<tr>";
        echo "<td><a href=\"ActorLinkResult.php?id=".$result["res"]."\">".$result['actorName']."</a></td>";
        //echo "<td>" . $result['actorName'] . "</td>";
        echo "<td>" . $result['role'] . "</td>";
        echo "</tr>";
    }

    echo "</table>";


?>
	<form method="post" action="ActorInfo.php">
        <br>
        <input type="Submit" value="Go back to make another search!">
    </form>
</div>
</div>
</body>


</html>