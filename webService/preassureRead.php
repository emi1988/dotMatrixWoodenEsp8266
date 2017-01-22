<?php

define ( 'MYSQL_HOST',      'localhost' );
define ( 'MYSQL_BENUTZER',  'web568' );
define ( 'MYSQL_KENNWORT',  '15241827' );
define ( 'MYSQL_DATENBANK', 'usr_web568_7' );

$db_link = mysql_connect (MYSQL_HOST, MYSQL_BENUTZER, MYSQL_KENNWORT);


$db_sel = mysql_select_db( MYSQL_DATENBANK )
   or die("Auswahl der Datenbank fehlgeschlagen");

$sinceTime = $_POST['sinceTime'];

//echo "sinceTime received: ";
//echo $sinceTime;
   
$sql = "SELECT * FROM dotMatrixWeather WHERE time >= $sinceTime";       

$db_erg = mysql_query( $sql );

$time  = array();
$preassure = array();

$arrayData = array();

while ($zeile = mysql_fetch_array( $db_erg, MYSQL_ASSOC))	//liste der koordinaten durchlaufen
{
	$time[]    = $zeile['time'];
	$preassure[]     = $zeile['preassure'];
	

}
$arrayData['time'] = $time;
$arrayData['preassure'] = $preassure;


echo json_encode($arrayData)

//echo json_encode($longitude);
//echo json_encode($latitude);
//echo json_encode($timestampGps);				
?>