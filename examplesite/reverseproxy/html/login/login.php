<?php
if($_SERVER['REQUEST_METHOD'] === 'POST') {
	if($_POST['user'] === 'hacker' && $_POST['password'] === "compass") {
	    /* Its important to stick to the order of the setcookie commands! */
		setcookie('LOGON', 'ok');
		setcookie('MOD_MSHIELD_USERNAME', $_POST['user']);
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
	    setcookie('LOGON', 'failed');
	    setcookie('MOD_MSHIELD_USERNAME', $_POST['user']);
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
