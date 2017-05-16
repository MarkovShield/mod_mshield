<!doctype html>
<html lang="en">
<head>
	<meta charset="utf-8" />
	<title>Chat</title>
	<link rel="stylesheet" href="/private/css/main.css">
	<link rel="stylesheet" href="/private/css/but.css">
</head>
<body>
	<div class="form-group">
		<form action="chat.php" method="post">
			<table class="table table-striped table-bordered table-condensed" style="width:100%">
				<tr>
					<td>Please enter your message: <input type="submit" value="Submit"></td>
				</tr>
				<tr>
					<td><textarea class="form-control" rows="5" cols="80" id="message" name="message"></textarea></td>
				</tr>
			</table>
		</form>
	</div>
	<div>
		<br><br>
		<h2>Your Message</h2>
		<table class="table table-striped table-bordered table-condensed" style="width:100%;">
			<tr>
				<td>
					<?php
					if ($_SERVER['REQUEST_METHOD'] === 'POST') {
						echo $_POST['message'];  //Output: myquery
					} else {
						echo $_GET['message'];  //Output: myquery
					}
					?>
				</td>
			</tr>
		</table>
	</div>
</body>
</html>
