<?php

$timeStamp = $_POST['timeStamp'];
$preassure = $_POST['preassure'];


define ( 'MYSQL_HOST',      'localhost' );
define ( 'MYSQL_BENUTZER',  'web568' );
define ( 'MYSQL_KENNWORT',  '15241827' );
define ( 'MYSQL_DATENBANK', 'usr_web568_7' );


$db_link = mysql_connect (MYSQL_HOST, MYSQL_BENUTZER, MYSQL_KENNWORT);


$db_sel = mysql_select_db( MYSQL_DATENBANK )
or die("Auswahl der Datenbank fehlgeschlagen");


//$sql = "SELECT * FROM position";        //daten ist tabelle

//$db_erg = mysql_query( $sql );

$eintrag = "INSERT INTO raspiGPScar
(time, preassure)
VALUES
('$timeStamp', '$preassure')";

$eintragen = mysql_query($eintrag);


if($eintragen == true)
   {
//echo '<b>';   
echo "Eintrag war erfolgreich. Eingetragener rapiTimestamp:";
echo $timeStampRapi;
//echo '</b>';
   }
else
   {
   echo "Fehler beim Speichern";
   }

//INSERT statement doesn't return a ressource, so "mysql_free_result" isn't necessary
//mysql_free_result($eintragen);

mysql_close($db_link);


?>