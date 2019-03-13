//import
var express = require('express');
var parser = require('body-parser');
var path = require('path');
var app = express();
var session = require('express-session');
//var debug = require('./routes/debug'); //directory definierung
//Body Parser
app.use(parser.urlencoded({ extended: false }))
app.use(parser.json())
//app.use('/debug',debug);

app.use(session({secret: 'Tracee',          //session var
                proxy: true,                //settings ...
                resave: true,
                saveUninitialized: true})); 
 
app.use(function(req,res,next){
    res.locals.userValue = null;
    next();
})
 //View Engine
app.set('view engine','ejs');
app.set('views',path.join(__dirname,'views'))
 
app.get('/',function(req,res){ //home page 
    res.render('index',{ //visualisation
        topicHead : 'DefectDeflect',
    });
    console.log('user accessing Home page');
});
//zeromq set up https://github.com/zeromq/zeromq.js/
var zmq = require('zeromq')
  , sock = zmq.socket('req');

sock.connect('tcp://192.168.178.33:3000');
console.log('Producer bound to port 3000');
sock.send('some work');
sock.on('message', function(msg){
    console.log('work: %s', msg.toString());
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
    req.session.name=req.body.fstring;
    console.log(req.session.name);
    res.render('index',{ //visualisation
        userValue : strings,
        topicHead : 'Reverse completed'
    });


app.get('/debug',function(req,res){ //debug page 
    res.render('debug',{ //visualisation
        topicHead : 'DefectDeflect',
    });
    console.log('user accessing Debug page');
});
// pass a local variable to the view
res.render('debug', { tracee: req.session.name }, function(err, html) {
    
  });     
});
app.listen(1337,function(){
    console.log('server running on port 1337');
})