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
      <div id="banner-content"><h1>Add Movie/Actor Relationship</h1>
      </div>
    </div>

    <div id="main-body">
    <?php
      $database = new mysqli('localhost', 'cs143', '', 'CS143');
    if($database->connect_errno > 0){
        die('Unable to connect to database [' . $database->connect_error . ']');
    }
    $movieQuery = $database->query("SELECT id, title, year FROM Movie order by title;");
    $movieList = "";
    $movieList = $movieList . "<option SELECTED value=\"-1\"> Select Movie </option>";
    while($row = $movieQuery->fetch_array(MYSQLI_ASSOC)){
      $id = $row["id"];
      $title = $row["title"];
      $year = $row["year"];
      $movieList = $movieList . "<option value=\"$id\"> $title ($year) </option>";

    }

    $actorQuery = $database->query("SELECT id, last, first, dob FROM Actor order by last;");
    $actorList = "";
    $actorList = $actorList . "<option value=\"-1\"> Select Actor</option>";
    while($row = $actorQuery->fetch_array(MYSQLI_ASSOC)){
      $last = $row["last"];
      $id = $row["id"];
      $first = $row["first"];
      $dob = $row["dob"];
      $actorList = $actorList . "<option value=\"$id\"> $first $last ($dob)</option>";

    }

?>
<form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
  <b>Movie</b><br>
  <SELECT name="movie">
    <?=$movieList?>
  </SELECT><br><br>
  <b>Actor</b><br>
  <SELECT name="actor">
    <?=$actorList?>
  </SELECT><br><br>

  <b>Role</b><br>
  <input type="text" name="role" size="50" maxlength="50" placeholder="e.g., cop"><br><br>
  <input type="submit" value="Add Relationship!">


</form>
<?php
  if (($_SERVER["REQUEST_METHOD"] == "GET") and (!empty($_GET))) {

  $quitError = 0;
  if(($movieID = $_GET['movie'] ) == "-1"){
    echo "Must select a movie!<br>";
    $quitError = 1;
  }
  if(($actorID = $_GET['actor'] ) == "-1"){
    echo "Must select an actor!<br>";
    $quitError = 1;
  }
  if(!($role = $_GET['role'])){
    echo "Must specify a role!<br>";
    $quitError = 1;
  }

  if(!$quitError){
    //do stuff we like
    $statement = $database->prepare("INSERT INTO MovieActor (mid, aid, role) VALUES (?, ?, ?);");
        $statement->bind_param("iis", $movieID, $actorID, $role);
        $statement->execute();
        echo "Success!";
  }

  }
?>

</div>
</div>
</body>
</html>