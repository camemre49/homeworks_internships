import { Injectable } from '@angular/core';
import {HttpClient, HttpHeaders, HttpErrorResponse} from '@angular/common/http'
import {catchError, map, Observable, of, tap, throwError} from "rxjs";
import {User} from "../../User";
import {Message} from "../../Message";
import {MenuService} from "./menu.service";


@Injectable({
  providedIn: 'root'
})
export class ApiService {
  private userUrl = "http://localhost:4000/api/user"
  private messageUrl = "http://localhost:4000/api/message"
  private username: string;
  private inOrOut: string;
  messagePageLimit: number = 50;
  userPageLimit: number = 50;
  logPageLimit: number = 30;
  constructor(private http:HttpClient, private menuService: MenuService) { }

  getHeader(){
    const httpOptions = {
      headers: new HttpHeaders({
        "Content-type": "application/json",
        "Authorization": sessionStorage.getItem("clientToken")
      })
    }
    return httpOptions;
  }

  getInOut(){
    return this.inOrOut
  }
  setUsername(username){

    this.username = username;
  }

  login(user): Observable<User>{
    let httpOptions = this.getHeader()
    return this.http.post<User>(this.userUrl + "/login", user, httpOptions).pipe(catchError(this.loginError));
  }


  create(user): Observable<User>{
    let httpOptions = this.getHeader()
    return this.http.post<User>(this.userUrl + "/create", user, httpOptions).pipe(tap(() => {

      }),
      catchError((error) => {
        this.handleError(error);
        this.createError(error);
        throw error
      }));
  }

  createError(error: HttpErrorResponse) {
    let errorMessage = '';
    if (error.error instanceof ErrorEvent) {
      // Get client-side error
      errorMessage = error.error.message;
    } else {
      // Get server-side error
      if(error.status == 405){
        alert("Username or email is used before!")
      }
    }
    return throwError(() => {
      return errorMessage;
    });
  }

  delete(user: String): Observable<any>{
    let httpOptions = this.getHeader()
    let url = this.userUrl + "/delete/"+user
    return this.http.delete(url, httpOptions).pipe(tap(() => {

      }),
      catchError((error) => {
        this.handleError(error);
        this.deleteError(error);
        throw error
      }))
  }

  deleteError(error: HttpErrorResponse){
    let httpOptions = this.getHeader()
    let errorMessage = '';
    if (error.error instanceof ErrorEvent) {
      // Get client-side error
      errorMessage = error.error.message;
    } else {
      // Get server-side error
      if(error.status == 404){
        alert("This user is already deleted!");
      }
    }
    console.log(errorMessage);
    return throwError(() => {
      return errorMessage;
    });

  }


  update(user: User): Observable<any>{
    let httpOptions = this.getHeader()
    return this.http.put(this.userUrl + "/update/"+user._id, user, httpOptions).pipe(tap(() => {

      }),
      catchError((error) => {
        this.handleError(error);
        this.createError(error);
        throw error
      }))
  }


  getUsers(query, page, criteria, reverse){
    const httpOptions = {
      headers: new HttpHeaders({
        "Content-type": "application/json",
        "Authorization": sessionStorage.getItem("clientToken"),
        "query" : query,
        "page" : page,
        "sort" : criteria,
        "limit": this.userPageLimit,
        "reverse" : reverse
      })
    }
    return this.http.get(this.userUrl, httpOptions).pipe(tap(() => {

      }),
      catchError((error) => {
        this.handleError(error);
        throw error
      }))
  }

  loginError(error: HttpErrorResponse){
    let errorMessage = '';
    if (error.error instanceof ErrorEvent) {
      // Get client-side error
      errorMessage = error.error.message;
    } else {
      // Get server-side error
      if(error.status == 404){
        alert("Username not found!");
      }
      else if(error.status == 401){
        alert("Wrong Password!");
      }
    }
    return throwError(() => {
      return errorMessage;
    });

  }

  loginControl(){
    let httpOptions = this.getHeader()
    return this.http.get(this.userUrl + "/loginCheck" , httpOptions).pipe(
      tap(() => {

      }),
      catchError((error) => {
        this.handleError(error);
        throw error
      })
    )
  }


  // Message Service
  sendMessage(message): Observable<Message>{
    let httpOptions = this.getHeader()
    message.sender = this.username;
    return this.http.post<Message>(this.messageUrl + "/send", message, httpOptions).pipe(tap(() => {

    }),
      catchError((error) => {
        this.handleError(error);
        if(error.status == 404){
          alert("Receiver not found!");
        }
        throw error
      })
    );
  }

  getMessages(page, criteria, rev){
    const httpOptions = {
      headers: new HttpHeaders({
        "Content-type": "application/json",
        "Authorization": sessionStorage.getItem("clientToken"),
        "page": page,
        "limit": this.messagePageLimit,
        "sort": criteria,
        "reverse": rev
      })
    }
    let url = this.messageUrl + "/" + this.inOrOut + "/" + this.username;
    return this.http.get(url, httpOptions).pipe(tap(() => {

      }),
      catchError((error) => {
        this.handleError(error);
        if(error.status == 500){
          alert("Unexpected error!")
        }
        throw error
      }));
  }
  inbox(){
      this.inOrOut = "inbox";
  }
  outbox(){
      this.inOrOut = "outbox"
  }

  deleteMessage(id: string) {
    let httpOptions = this.getHeader()
    let url = this.messageUrl + "/" + this.inOrOut + "/" + id;
    return this.http.delete(url, httpOptions).pipe(
      tap(() => {
        // Code to handle successful completion (if needed)
      }),
      catchError((error) => {
        this.handleError(error);
        if(error.status == 500){
          alert("Unexpected error!")
        }
        throw error
      })
    );
  }

  getLog(query, page, criteria, reverse){
    const httpOptions = {
      headers: new HttpHeaders({
        "Content-type": "application/json",
        "Authorization": sessionStorage.getItem("clientToken"),
        "query": query,
        "page": page,
        "limit": this.logPageLimit,
        "sort": criteria,
        "reverse": reverse
      })
    }
    return this.http.get("http://localhost:4000/api/log", httpOptions).pipe(
      tap(() => {
        // Code to handle successful completion (if needed)
      }),
      catchError((error) => {
        this.handleError(error);
        throw error
      })
    )
  }


  logout(){
    this.setUsername("");
    return this.http.get(this.userUrl + "/logout", this.getHeader()).pipe()
  }

  handleError(error: HttpErrorResponse) {
    let errorMessage = '';
    if (error.error instanceof ErrorEvent) {
      errorMessage = error.error.message;
    } else {
      if(error.status == 403){
        this.menuService.logout();
      }
      else if(error.status == 406){
        alert("You are not registered!")
        this.menuService.logout();
      }
      else if(error.status == 401){
        alert("You are not allowed!")
      }
      else if(error.status == 500){
        alert("An unexpected error occured!")
        this.menuService.logout();
      }
    }
    return throwError(() => {
      return errorMessage;
    });
  }


}
