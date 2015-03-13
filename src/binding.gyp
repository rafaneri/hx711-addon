{
	"targets":[
		{
			"target_name": "hx711",
			"sources": ["hx711.cc", "HX711.cpp"],
			"ldflags": ["-lmraa", "-unistd=libc++"],
	    	"cflags": ["-fexceptions"],
    	  	"cflags_cc": ["-fexceptions"]
		}
	]
}
