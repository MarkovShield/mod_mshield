<?php
if($_SERVER['REQUEST_METHOD'] === 'POST') {
	if($_POST['user'] === 'hacker' && $_POST['password'] === "compass") {
		setcookie('LOGON', 'ok');
		setcookie('MOD_MSHIELD_USERNAME', 'hacker');
		setcookie('MOD_MSHIELD_AUTH_STRENGTH', '2');
		switch ($_POST['appid']) {
		    case 1:
		        setrawcookie('MOD_MSHIELD_REDIRECT', '/private/1/');
		        break;
		    case 2:
		        setrawcookie('MOD_MSHIELD_REDIRECT', '/private/2/');
		        break;
		    default:
		       	setrawcookie('MOD_MSHIELD_REDIRECT', '/private/');
		}
		die();
	} else {
		header('Location: /login/login0.html');
		die();
	}
} else {
		header('Location: /login/login0.html');
		die();
	}
?>
<html>
<body>
grrr
</body>
</html>
