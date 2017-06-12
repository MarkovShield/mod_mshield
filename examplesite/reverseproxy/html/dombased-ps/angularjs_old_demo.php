<html>
<head>
<title>Outdated AngularJS Demo</title>
</head>
<body style="font-family:verdana">	
<script src="http://ajax.googleapis.com/ajax/libs/angularjs/1.1.5/angular.min.js"></script>
<div class="ng-app">
<h1>USER INPUT IN TEMPLATES</h1>
	<form action="angularjs_old_demo.php" method="GET">
	Search:&nbsp;<input type="text" name="search"/></br></br>
	<button type="submit">Send</button>
	</br></br>
	<?php if(isset($_GET['search'])){echo "Your search for <b>\"".htmlentities($_GET['search'])."\"</b> did not return any results";}?>
	</form>
       </div>
	   </body>
</html>
