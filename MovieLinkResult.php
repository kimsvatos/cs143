<html>
<body>


<h1>Movie Information Page:</h1>
<!-- do queries and show table with DOB name DOD blah --> 
<h3> Movie Information is:</h3>
<?php
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
    echo "<table border='1' bordercolor='black' cellpadding='2'>";
    echo "<tr>";
    echo "<th>Actor Name</th>";
    echo "<th>Role</th>";
    echo "</tr>";

    $id_query = "SELECT CONCAT(a.first,' ',a.last) actorName, a.id res, ma.role FROM Actor a, Movie m, MovieActor ma WHERE m.id = $id and ma.mid = m.id and ma.aid = a.id;";
    $queryRes = $database->query($id_query);
    while($result = $queryRes->fetch_array(MYSQLI_ASSOC)){
        echo "<tr>";
        echo "<a href=\"ActorLinkResult.php?id=".$result["res"]."\">".$result['actorName']."</a>";
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

</body>


</html>