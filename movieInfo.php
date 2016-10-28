<html>
<title>i hate kyle</title>
	<h1 color="blue"> Add Movie Information</h1>
	<body>
	  <form method="get" action="<?php echo $_SERVER['PHP_SELF'];?>">
     
      <br>
      Title
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

      Rating:<SELECT NAME="Rating">
      <OPTION> G
      <OPTION> PG
      <OPTION> PG-13
      <OPTION> R
      <OPTION> NC-17
      <OPTION> surrendere
      </SELECT>

      <br>
      <marquee speed=3>Action<input type="checkbox" name="action" value="ON"></marquee>
 	  Adult<input type="checkbox" name="adult" value="ON">
      Adventure<input type="checkbox" name="adventure" value="ON">
 	  Animation<input type="checkbox" name="animation" value="ON">
 	  Comedy<input type="checkbox" name="comedy" value="ON"><br>
 	  Crime<input type="checkbox" name="crime" value="ON">
      Documentary<input type="checkbox" name="documentary" value="ON">
 	  Drama<input type="checkbox" name="drama" value="ON">
 	  Family<input type="checkbox" name="family" value="ON"><br>
 	  <marquee speed=5>Fantasy<input type="checkbox" name="fantasy" value="ON">
      Horror<input type="checkbox" name="horror" value="ON">
 	  Musical<input type="checkbox" name="musical" value="ON"></marquee>
 	  Mystery<input type="checkbox" name="mystery" value="ON"><br>
 	  <marquee speed=1>Romance<input type="checkbox" name="romance" value="ON">
      Sci-Fi<input type="checkbox" name="scifi" value="ON"></marquee>
 	  Short<input type="checkbox" name="short" value="ON">
 	  Thriller<input type="checkbox" name="thriller" value="ON"><br>
 	  War<input type="checkbox" name="war" value="ON">
      Western<input type="checkbox" name="western" value="ON">
 	  <br>
      <input type="submit" value="Submit">
   </form>
	</body>




</html>