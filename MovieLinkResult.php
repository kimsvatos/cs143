<!DOCTYPE html>
<html>
  <head>
    <title>
      CS143 Movie Database
    </title>
    <style>
      body {
          margin: 0;
          padding: 0;
          font-family: "Arial", sans-serif;
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

      div#banner {
        display: block;
        padding: 0px 15px;
        position: fixed;
        top: 0;
        left: 25%;
        width: 100%;
        background-color: #065790;
      }

      div#main-body {
        padding-top: 95px;
      }

      h1 {
        color: white;
        padding: 1px 1px;
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

      input[type=submit] {
        background: #CCCCCC;
        border: 1px solid #000000;
        border-radius: 25px;
        width: 150px;
        height: 30px;
        font-family: "Arial", sans-serif;
        font-size: 1.02em;
      }

      input[type=submit]:hover {
        background: #66B3FF;
        color: white;
      }

      tr:hover{background-color:#f5f5f5}
      input[type=submit] {
        background: #CCCCCC;
        border: 1px solid #000000;
        border-radius: 25px;
        width: 130px;
        height: 28px;
        font-family: "Arial", sans-serif;
        font-size: 11px; 
        font-weight: bold;
      }

      input[type=submit]:hover {
        background: #66B3FF;
        color: white;
        font-weight: bold;
      }

      
    </style>
  </head>
  <body>
    <ul>
      <li><a class="home" href="./index.php">Home</a></li>
      <li><a class="header">Add Content</a></li>
      <li><a href="./test.php">Add New Actor/Director</a></li>
      <li><a href="./movieInfo.php">Add New Movie</a></li>
      <li><a class="active" href="./movieActor.php">Add Movie/Actor Relationship</a></li>
      <li><a href="./movieDirector.php">Add Movie/Director Relationship</a></li>
      <li><a href="./review.php">Add Review</a></li>
      <li><a class="header">Search Content</a></li>
      <li><a href="./ActorInfo.php">Search Actors</a></li>
      <li><a href="./MovieSearchPage.php">Search Movies</a></li>
      <li><a href="./GenSearch.php">Search All</a></li>
    </ul>

    <div style="margin-left:25%; padding:1px 16px; height:auto;">
    <div id="banner">
      <div id="banner-content"><h1>Movie Information</h1>
      </div>
    </div>

    <div id="main-body">
<body>


<ul>
  <li><a href="./index.php">Home</a></li>
  <li><a href="./movieInfo.php">Insert a New Actor/Director</a></li>
  <li><a href="./movieInfo.php">Insert a New Movie</a></li>
  <li><a href="./movieActor.php">Add a Movie/Actor relationship!</a></li>
  <li><a href="./movieDirector.php">Add a Movie/Director relationship!</a></li>
  <li><a href="./review.php">Add a Review</a></li>
  <li><a href="./ActorInfo.php">Actor Lookup</a></li>
  <li><a href="./MovieSearchPage.php">Movie Lookup</a></li>
  <li><a href="./GenSearch.php">General Search</a></li>
 
</ul>
<div style="margin-left:25%;padding:1px 16px;height:1000px;">



<h1>Movie Information Page:</h1>
<!-- do queries and show table with DOB name DOD blah --> 
<h3> Movie Information is:</h3>
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


    echo "Title: " . $title . " (" . $year . ")<br>";
    echo "Company: " . $comp . "<br>";
    echo "Rating: " . $rating . "<br>";
    if($dir == "0" || $dir == NULL){
        echo "Director: <br>";
    }
    else{
        echo "Director: " . $dir . " (" . $dob . ")<br>";
    }
    
    echo "Genre: " . $genre . "<br>";
    echo "<h1> Actors in this movie: </h1>";
    echo "<table cellpadding='2'>";
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
/*
	//movie info 
	$id_query = "SELECT role, title from MovieActor, Movie WHERE aid = $id AND mid=id";
    $queryRes = $database->query($id_query);
    
	///next table
    echo "<h1> Actor's Movies and Role:</h1>";


    echo "<table border='1' bordercolor='black' cellpadding='2'>";
    echo "<tr>";
    echo "<th>Movie Title</th>";
    echo "<th>Role</th>";
   
    echo "</tr>";

    while($result = $queryRes->fetch_array(MYSQLI_ASSOC)){
    echo "<tr>";
    echo "<td>" . $result['title'] . "</td>"; //this will link to MOVIElinkResult
	echo "<td>" . $result['role'] . "</td>";
	
	echo "</tr>";
	}	

	echo "</table>";
   // echo "</table>";*/

?>
	<form method="post" action="ActorInfo.php">
        <input type="Submit" value="Go back to make another search!">
    </form>
</div>
</div>
</body>


</html>