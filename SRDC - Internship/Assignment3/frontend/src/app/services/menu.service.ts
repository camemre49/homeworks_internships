import { Injectable } from '@angular/core';
import {Observable, Subject, Subscription} from "rxjs";
import {Router} from "@angular/router";

@Injectable({
  providedIn: 'root'
})
export class MenuService {
  subject: Subject<any> = new Subject<any>()
  loggedIn: boolean = false;
  private adminStatusSubject = new Subject<boolean>();
  private totalPageSubject = new Subject<number>();
  adminStatus = this.adminStatusSubject.asObservable();
  admin: boolean;
  pageLimit:number = 100;
  inboxItem: number = 0;
  outboxItem: number= 0;

  constructor(private router: Router) { }

  setInboxItem(nu){
    this.inboxItem = nu;
  }
  setOutboxItem(nu){
    this.outboxItem = nu;
  }
  loginInfo(loginSuccessfull: boolean){
    this.subject.next(loginSuccessfull);
    this.loggedIn = true;
  }

  getLoginInfo(){
    return this.subject.asObservable();
  }

  getLoggedIn(){
    return this.loggedIn;
  }

  setAdmin(bool){
    this.adminStatusSubject.next(bool);
    this.admin = bool;
  }

  getAdmin(){
    return this.admin;
  }


  turnMenu(){
    this.router.navigate([""])
  }

  logout(){
    this.loggedIn = false;
    this.subject.next(false);
  }
}
