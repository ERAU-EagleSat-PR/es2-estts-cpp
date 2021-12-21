## Contributing

write useful code

## Logging best practices

_Logging best practices borrowd
from_ [the kind people at SentinelOne](https://www.sentinelone.com/blog/the-10-commandments-of-logging/)

* TRACE level: this is a code smell if used in production. This should be used during development to track bugs, but
  never committed to your VCS.
* DEBUG level: log at this level about anything that happens in the program. This is mostly used during debugging, and
  I’d advocate trimming down the number of debug statement before entering the production stage, so that only the most
  meaningful entries are left, and can be activated during troubleshooting.
* INFO level: log at this level all actions that are user-driven, or system specific (ie regularly scheduled
  operations…)
* NOTICE level: this will certainly be the level at which the program will run when in production. Log at this level all
  the notable events that are not considered an error.
* WARN level: log at this level all events that could potentially become an error. For instance if one database call
  took more than a predefined time, or if an in-memory cache is near capacity. This will allow proper automated
  alerting, and during troubleshooting will allow to better understand how the system was behaving before the failure.
* ERROR level: log every error condition at this level. That can be API calls that return errors or internal error
  conditions.
* FATAL level: too bad, it’s doomsday. Use this very scarcely, this shouldn’t happen a lot in a real program. Usually
  logging at this level signifies the end of the program. For instance, if a network daemon can’t bind a network socket,
  log at this level and exit is the only sensible thing to do.
* 