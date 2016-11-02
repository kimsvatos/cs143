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
      <li><a class="active" class="home" href="./index.php">Home</a></li>
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
      <div id="banner-content"><h1>Welcome to the Svaacker Movie Database!</h1>
      </div>
    </div>

    <div id="main-body">
  <body>




     
     Please select from an option on the left! You may add a person, movie, or relationship to our database, <br>
     write a review, or do a search on movies, actors or people to discover films and roles!
<br><br><br><br><br>
      Database made for UCLA CS 143, by Kyle Haacker and Kim Svatos


</div>
</div>















    </div>









  </body>
</html>