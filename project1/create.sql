/**
 ** create.sql
 **
 ** Script to create tables containing
 ** information about movies, actors,
 ** directors, reviews, etc.
 **
 **/

-- Constraint info for Movie:
   -- (1) Movie IDs must be unique (i.e., two movies cannot be identified by the same number)
   -- (2) Movie ratings must fall under the set of ratings defined by MPAA
CREATE TABLE Movie (
  id      INT
 ,title   VARCHAR(100)
 ,year    INT
 ,rating  VARCHAR(10)
 ,company VARCHAR(50)
 ,PRIMARY KEY (id) -- Constraint (1), see descriptions above
 ,CHECK (rating IS NULL OR rating IN ('G', 'PG', 'PG-13', 'R', 'NC-17')) -- Constraint (2)
) ENGINE = INNODB;


-- Constraint info for Actor:
   -- (3) Actor IDs must be unique (i.e., two people cannot be identified by the same number)
   -- (4) Actor must have been born. If actor has died, they must have died after they were born
CREATE TABLE Actor (
  id    INT
 ,last  VARCHAR(20)
 ,first VARCHAR(20)
 ,sex   VARCHAR(6)
 ,dob   DATE
 ,dod   DATE
 ,PRIMARY KEY (id) -- Constraint (3), see descriptions above
 ,CHECK (dob IS NOT NULL AND (dod IS NULL OR dod >= dob)) -- Constraint (4)
) ENGINE = INNODB;


-- Constraint info for Director:
   -- (5) Director IDs must be unique (i.e., two people cannot be identified by the same number)
   -- (6) Director must have been born. If they have died, they must have died after being born
CREATE TABLE Director (
  id    INT
 ,last  VARCHAR(20)
 ,first VARCHAR(20)
 ,dob   DATE
 ,dod   DATE
 ,PRIMARY KEY (id) -- Constraint (5), see descriptions above
 ,CHECK (dob IS NOT NULL AND (dod IS NULL OR dod >= dob)) -- Constraint (6)
) ENGINE = INNODB;


-- Constraint info for MovieGenre:
   -- (7) Each entry in MovieGenre must reference a valid Movie (i.e., the id exists in Movie)
CREATE TABLE MovieGenre (
  mid   INT
 ,genre VARCHAR(20)
 ,FOREIGN KEY (mid) REFERENCES Movie (id) -- Constraint (7), see description above
) ENGINE = INNODB;


-- Constraint info for MovieDirector:
   -- (8) MovieDirector entries must reference a valid Movie (i.e., the id exists in Movie)
   -- (9) MovieDirector entries must reference a valid Director (i.e., the id exists in Director)
CREATE TABLE MovieDirector (
  mid INT
 ,did INT
 ,FOREIGN KEY (mid) REFERENCES Movie (id) -- Constraint (8), see descriptions above
 ,FOREIGN KEY (did) REFERENCES Director (id) -- Constraint (9)
) ENGINE = INNODB;


-- Constraint info for MovieActor:
   -- (10) MovieActor entries must reference a valid Movie (i.e., the id exists in Movie)
   -- (11) MovieActor entries must reference a valid Actor (i.e., the id exists in Actor)
CREATE TABLE MovieActor (
  mid  INT
 ,aid  INT
 ,role VARCHAR(50)
 ,FOREIGN KEY (mid) REFERENCES Movie (id) -- Constraint (10), see descriptions above
 ,FOREIGN KEY (aid) REFERENCES Actor (id) -- Constraint (11)
) ENGINE = INNODB;


-- Constraint info for Review
   -- (12) Review entries must reference a valid Movie (i.e., the id exists in Movie)
   -- (13) Review ratings must fall between 0 and 5, as stated in the spec
CREATE TABLE Review (
  name    VARCHAR(20)
 ,time    TIMESTAMP
 ,mid     INT
 ,rating  INT
 ,comment VARCHAR(500)
 ,FOREIGN KEY (mid) REFERENCES Movie (id) -- Constraint (12), see descriptions above
 ,CHECK (rating >= 0 AND rating <= 5) -- Constraint (13)
) ENGINE = INNODB;

CREATE TABLE MaxPersonID (
  id INT
) ENGINE = INNODB;

CREATE TABLE MaxMovieID (
  id INT
) ENGINE = INNODB;