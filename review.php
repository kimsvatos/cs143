<html>
<title>CS 143 Movie Database</title>
	<h1>Review a Movie!</h1>
	<body>
	  <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
     
      <br>
      Your Name:
      <br>
      <INPUT TYPE="text" name="title">
      <br><br>
      Company
      <br>
      <input type="text" name="company">
      <br>
      Year
      <br>
      <input type="text" name="year">
      <br>

      Rating:<SELECT NAME="rating">
      <OPTION SELECTED> G
      <OPTION> PG
      <OPTION> PG-13
      <OPTION> R
      <OPTION> NC-17
      <OPTION> surrendere
      </SELECT>


 	  <br>
      <input type="submit" value="Submit">
   </form>



	</body>




</html>