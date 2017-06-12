<?php
	function generateRandomString($length = 10) {
    $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
    $charactersLength = strlen($characters);
    $randomString = '';
    for ($i = 0; $i < $length; $i++) {
        $randomString .= $characters[rand(0, $charactersLength - 1)];
    }
    return $randomString;
}
	$value = md5(generateRandomString());
	setcookie("XSRF-TOKEN", $value);
?>
<html>
	<head>
		<title>AngularJS Demo</title>
		 <script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.4.5/angular.min.js"></script>
		 <script src="https://ajax.googleapis.com/ajax/libs/angularjs/1.4.5/angular-sanitize.js"></script>
		 <script>
			var myModule = angular.module('myModule', ['ngSanitize']);
			myModule.controller('myController', ['$scope', '$sce', '$http', function($scope, $sce, $http){
				$scope.deliberatelyTrustDangerousSnippet = function() {
					return $sce.trustAsHtml($scope.snippet);
				}
				$scope.testFunction = function(userInput){
					eval(userInput);
				}
				$scope.formData = {};
				$scope.processForm = function(){
					$http({
  						method  : 'POST',
  						url     : 'angularjs_demo.php',
  						data    : $scope.formData,  // pass in data as strings
  						headers : {'Content-Type': 'application/x-www-form-urlencoded'}  // set the headers so angular passing info as form data (not request payload)
					 })
				};	
			}]);	
     		</script>
	</head>
<body ng-app="myModule" ng-controller="myController" style="font-family:verdana">
     <div>
	<h1>OUTPUT ENCODING</h1>
        Snippet: <input type="text" ng-model="snippet" size=100 />
		</br>
		</br>
       <table>
         <tr>
           <td width=120><b>Directive</b></td>
           <td width=400><b>How</b></td>
           <td width=500><b>Source</b></td>
           <td><b>Rendered</b></td>
         </tr>
		 	     <tr id="bind-default">
           <td>ng-bind</td>
           <td>Automatically escapes</td>
           <td><pre>&lt;div ng-bind="snippet"&gt;<br/>&lt;/div&gt;</pre></td>
           <td><div ng-bind="snippet"></div></td>
         </tr>
         <tr id="bind-html-with-sanitize">
           <td>ng-bind-html</td>
           <td>Automatically uses $sanitize</td>
           <td><pre>&lt;div ng-bind-html="snippet"&gt;<br/>&lt;/div&gt;</pre></td>
           <td><div ng-bind-html="snippet"></div></td>
         </tr>
         <tr id="bind-html-with-trust">
           <td>ng-bind-html</td>
           <td>Bypass $sanitize by explicitly trusting the dangerous value</td>
           <td>
           <pre>&lt;div ng-bind-html="deliberatelyTrustDangerousSnippet()"&gt;
&lt;/div&gt;</pre>
           </td>
           <td><div ng-bind-html="deliberatelyTrustDangerousSnippet()"></div></td>
         </tr>
       </table>
	   <hr>
	<h1>CSRF PROTECTION</h1>
	   <form ng-submit="processForm()">
	   	Input:&nbsp;<input type="text" name="input" ng-model="formData.input"/>
		</br>
		</br>
		<button type="submit">Send</button>
	</form>
	<hr>
	<h1>USER INPUT IN TEMPLATES</h1>
	<form action="angularjs_demo.php" method="GET">
	Search:&nbsp;<input type="text" name="search"/></br></br>
	<button type="submit">Send</button>
	</br></br>
	<?php if(isset($_GET['search'])){echo "Your search for <b>".htmlentities($_GET['search'])."</b> did not return any results";}?>
	</form>
       </div>
	   </body>
</html>
