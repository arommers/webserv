const delayedGreeting = function(name, delay)
{
    return new Promise(function(resolve, reject)
    {
        setTimeout(()=> 
        {
            resolve(`Hello ${name}`)
        }, delay);
    });
}

delayedGreeting('Snoet', 2000)
.then(data => console.log(data))
.catch(error => console.log(new Error(error)));
