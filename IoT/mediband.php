<?php
$usuario = "sql9614594";
$contrasena = "uKnlFW8Zwu";
$servidor = "sql9.freemysqlhosting.net";
$baseDeDatos = "sql9614594";

$conexion = mysqli_connect($servidor, $usuario, $contrasena, $baseDeDatos);

$id = $_GET["id"];

$consulta = "SELECT CURPpaciente FROM Brazalete WHERE IDbrazalete = '".$id."'";
$resultado = mysqli_query($conexion, $consulta);
$respuesta = mysqli_fetch_assoc($resultado);
$curp = $respuesta["CURPpaciente"];

$temperatura = $_GET["temperatura"];
$pulso = $_GET["pulso"];
$ubicacion = $_GET["ubicacion"];

$consulta1 = "INSERT INTO Temperatura (Grados, CURPpaciente) VALUES ('".$temperatura."','".$curp."')";
mysqli_query($conexion,$consulta1);

$consulta2 = "INSERT INTO Pulso (PPM, CURPpaciente) VALUES ('".$pulso."','".$curp."')";
mysqli_query($conexion,$consulta2);

$consulta3 = "INSERT INTO Ubicacion (Coordenadas, CURPpaciente) VALUES ('".$ubicacion."','".$curp."')";
mysqli_query($conexion,$consulta3);

//https://iotequipo3.000webhostapp.com/mediband.php?id=0&temperatura=35&pulso=99&ubicacion=42394
?>