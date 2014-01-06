<?php

header('Content-Type: text/html; charset=utf-8');

session_start();
require_once("twitteroauth/twitteroauth.php"); //Path to twitteroauth library
 
$search = "#tweetball";
$notweets = 1;
$consumerkey = "your consumer key";
$consumersecret = "your consumer secure";
$accesstoken = "your access token";
$accesstokensecret = "your token secret";

function getConnectionWithAccessToken($cons_key, $cons_secret, $oauth_token, $oauth_token_secret) {
  $connection = new TwitterOAuth($cons_key, $cons_secret, $oauth_token, $oauth_token_secret);
  return $connection;
}
   
$connection = getConnectionWithAccessToken($consumerkey, $consumersecret, $accesstoken, $accesstokensecret);
 
$search = str_replace("#", "%23", $search);
$tweets = $connection->get("https://api.twitter.com/1.1/search/tweets.json?q=".$search."&count=".$notweets);


$randomTweet = rand(0,$notweets-1);
$parseTweet = $tweets->statuses[$randomTweet]->text;
$parseTweet = "!".$parseTweet."~";

$parseTweet = str_replace("\n","", $parseTweet);

echo htmlspecialchars_decode($parseTweet);

?>