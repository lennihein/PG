//import
var express = require('express');
var parser = require('body-parser');
var path = require('path');
var app = express();
//Body Parser
app.use(parser.urlencoded({ extended: false }))
app.use(parser.json())
 
app.use(function(req,res,next){
    res.locals.userValue = null;
    next();
})
 //View Engine
app.set('view engine','ejs');
app.set('views',path.join(__dirname,'views'))
 
app.get('/',function(req,res){ //home page 
    res.render('index',{ //visualisation
        topicHead : 'Reverse String',
    });
    console.log('user accessing Home page');
});

function reverseString(str) {
    var splitString = str.split(""); //string to array
    var reverseArray = splitString.reverse(); //reverse array
    var joinArray = reverseArray.join(""); //array to string
    return joinArray;
}

app.post('/reverse',function(req,res){ //executed after input
    var strings = { //array of two strings
        first : reverseString(req.body.fstring),
        last : reverseString(req.body.lstring)
    }
    console.log(strings);
    res.render('index',{ //visualisation
        userValue : strings,
        topicHead : 'Reverse completed'
    });
     
});
app.listen(1337,function(){
    console.log('server running on port 1337');
})